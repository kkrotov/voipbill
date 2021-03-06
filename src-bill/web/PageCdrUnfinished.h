#pragma once

#include "BasePage.h"
#include "../classes/Repository.h"
#include "../classes/DbException.h"

class PageCdrUnfinished : public BasePage {
    Spinlock lock;
    Repository repository;

public:
    string time_suffix(time_t time) {

        char buff[16];
        struct tm * timeinfo = gmtime(&time);
        strftime(buff, sizeof(buff), "%Y%m", timeinfo);
        return string(buff);
    }

    string sql_time(time_t time) {

        char buff[128];
        struct tm * timeinfo = gmtime(&time);
        strftime(buff, sizeof(buff), "%Y-%m-%d %H:%M:%S", timeinfo);
        return string(buff);
    }

    string str_date (time_t time) {

        char buff[128];
        struct tm * timeinfo = gmtime(&time);
        strftime(buff, sizeof(buff), "%Y-%m-%d", timeinfo);
        return string(buff);
    }

    string str_time (time_t time) {

        char buff[128];
        struct tm * timeinfo = gmtime(&time);
        strftime(buff, sizeof(buff), "%H:%M:%S", timeinfo);
        return string(buff);
    }

    time_t uri_data_time (string uriDate, string uriTime) {

        struct tm uri_tm = {0};
        strptime(uriTime.c_str(), "%H:%M:%S", &uri_tm);

        struct tm uri_dt = {0};
        if (sscanf(uriDate.c_str(), "%d-%d-%d", &uri_dt.tm_year, &uri_dt.tm_mon, &uri_dt.tm_mday) <= 0)
            return time(NULL);

        uri_dt.tm_year -= 1900;
        uri_dt.tm_mon -= 1;
        uri_dt.tm_hour = uri_tm.tm_hour;
        uri_dt.tm_min = uri_tm.tm_min;
        uri_dt.tm_sec = uri_tm.tm_sec;
        return timegm(&uri_dt);
    }

    bool getCdrUnfinished(BDb * db_calls, time_t timeFrom, time_t timeTo, string src_number, string dst_number, string limit, string showFinished, vector<Cdr> &cdrUnfinished) {

        if (timeFrom > timeTo)
            return false;

        string cdr_unfinished = string("calls_cdr.cdr_unfinished_")+time_suffix(timeFrom);
        string calls_cdr = string("calls_cdr.cdr_")+time_suffix(timeFrom);
        string sql_join = " full outer join ";
        bool show_finished_only = false;
        bool show_unfinished_only = false;
        if (showFinished.compare("unfin")==0) {

            show_unfinished_only = true;
            sql_join = " left join ";
        }
        if (showFinished.compare("fin")==0) {

            sql_join = " right join ";
            show_finished_only = true;
        }
        string select =
                "select "
                    "coalesce("+cdr_unfinished+".call_id, "+calls_cdr+".call_id) as _call_id_, "+           // 0
                    "coalesce("+cdr_unfinished+".setup_time, "+calls_cdr+".setup_time) as _setup_time_, "+  // 1
                    "coalesce("+cdr_unfinished+".src_number, "+calls_cdr+".src_number), "+      // 2
                    "coalesce("+cdr_unfinished+".dst_number, "+calls_cdr+".dst_number), "+      // 3
                    "coalesce("+cdr_unfinished+".src_route, "+calls_cdr+".src_route), "+        // 4
                    cdr_unfinished+".dst_route, "+                  // 5
                    calls_cdr+".dst_route, "+                       // 6
                    "coalesce("+cdr_unfinished+".releasing_party, "+calls_cdr+".releasing_party), "+        // 7
                    cdr_unfinished+".release_timestamp, "+          // 8
                    cdr_unfinished+".disconnect_cause, "+           // 9
                    calls_cdr+".redirect_number, "+                 // 10
                    calls_cdr+".connect_time, "+                    // 11
                    calls_cdr+".disconnect_time, "+                 // 12
                    calls_cdr+".session_time, "+                    // 13
                    calls_cdr+".src_noa, "+                         // 14
                    calls_cdr+".dst_noa, "+                         // 15
                    calls_cdr+".dst_replace, "+                     // 16
                    calls_cdr+".disconnect_cause "+                 // 17
                "from "+cdr_unfinished+ sql_join+calls_cdr+" using(hash) ";

        string cdr_time = "coalesce("+cdr_unfinished+".setup_time, "+calls_cdr+".setup_time)";
        string cdr_src = "coalesce("+cdr_unfinished+".src_number, "+calls_cdr+".src_number)";
        string cdr_dst = "coalesce("+cdr_unfinished+".dst_number, "+calls_cdr+".dst_number)";
        string where = "where "+cdr_time+">='"+sql_time(timeFrom)+"' and "+cdr_time+"<='"+sql_time(timeTo)+"' ";
        if (src_number.size()>0)
            where += " and "+cdr_src+" like '%"+src_number+"%'";

        if (dst_number.size()>0)
            where += " and "+cdr_dst+" like '%"+dst_number+"%'";

        if (show_unfinished_only)
            where += " and "+calls_cdr+".connect_time is null ";

        string order = "order by _call_id_ desc, _setup_time_ desc limit "+limit;

        try {

            BDbResult res = db_calls->query(select+where+order);
            if (res.size() < 0)
                return false;

            lock_guard<Spinlock> guard(lock);

            long long int current_call_id = 0;
            while (res.next()) {

                long long int call_id = res.get_ll(0);
                time_t setup_time = parseDateTime(res.get(1));
                char src_number[33];
                strcpy((char *) &src_number, res.get(2));
                char dst_number[33];
                strcpy((char *) &dst_number, res.get(3));
                char src_route[33];
                strcpy((char *) &src_route, res.get(4));
                char dst_route_unfinished[33];
                strcpy((char *) &dst_route_unfinished, res.get(5));
                char dst_route[33];
                strcpy((char *) &dst_route, res.get(6));
                char releasing_party_unfinished[32];
                strncpy((char*) &releasing_party_unfinished, res.get(7), sizeof(releasing_party_unfinished));
                time_t releasing_time = parseDateTime(res.get(8));
                int disconnect_cause_unfinished = res.get_i(9);

                char redirect_number[33];
                strcpy((char *) &redirect_number, res.get(10));
                time_t connect_time = parseDateTime(res.get(11));
                time_t disconnect_time = parseDateTime(res.get(12));
                time_t session_time = res.get_i(13);
                int src_noa = res.get_i(14);
                int dst_noa = res.get_i(15);
                char dst_replace[33];
                strcpy((char*)&dst_replace, res.get(16));
                int disconnect_cause_finished = res.get_i(17);

                if (call_id != current_call_id) {

                    if (connect_time > 0) {

                        Cdr cdr;
                        cdr.call_id = call_id;
                        strcpy((char *) &cdr.dst_route, dst_route);
                        cdr.disconnect_cause = disconnect_cause_finished;
                        strcpy((char *) &cdr.src_number, src_number);
                        strcpy((char *) &cdr.dst_number, dst_number);
                        strcpy((char *) &cdr.redirect_number, redirect_number);
                        cdr.connect_time = connect_time;
                        cdr.setup_time = setup_time;
                        cdr.session_time = session_time;
                        strcpy((char *) &cdr.src_route, src_route);
                        cdr.src_noa = src_noa;
                        cdr.dst_noa = dst_noa;
                        strcpy((char *) &cdr.dst_replace, dst_replace);
                        strncpy((char *) &cdr.releasing_party, releasing_party_unfinished, sizeof(cdr.releasing_party));

                        cdrUnfinished.push_back(cdr);
                    }
                }
                if (disconnect_cause_unfinished>0 && !show_finished_only) {

                    Cdr cdr;
                    cdr.call_id = call_id;
                    cdr.setup_time = setup_time;
                    strcpy((char *) &cdr.src_number, src_number);
                    strcpy((char *) &cdr.dst_number, dst_number);
                    strcpy((char *) &cdr.src_route, src_route);
                    strcpy((char *) &cdr.dst_route, dst_route_unfinished);
                    strncpy((char *) &cdr.releasing_party, releasing_party_unfinished, sizeof(cdr.releasing_party));
                    cdr.disconnect_cause = disconnect_cause_unfinished;
                    cdr.session_time = releasing_time>0? releasing_time - setup_time:0;

                    cdrUnfinished.push_back(cdr);
                }
                current_call_id = call_id;
            }
        }
        catch (DbException &e) {

            Log::error(e.what());
            return false;
        }
        return true;
    }

    bool canHandle(std::string &path) {
        return path == "/cdrunfinished";
    }

    void render(std::stringstream &html, map<string, string> &parameters) {

        if (!ready())
            return;

        html << "<!DOCTYPE html>\n" << "<html lang=\"en\">\n";
        renderHeader("cdrunfinished", html);

        string dateFrom = parameters["date_from"];
        string timeFrom = parameters["time_from"];
        string timeTo = parameters["time_to"];

        time_t time_from = uri_data_time(dateFrom, timeFrom);
        if (timeFrom.size()==0)
            time_from -= 3600;

        time_t time_to = uri_data_time(dateFrom, timeTo); //uri_time(parameters["date_to"].c_str());
        
        string src_number = parameters["src_number"];
        string dst_number = parameters["dst_number"];
        string tab_show = parameters["show"];
        string limit = parameters["limit"];

        string action = parameters["do_search"];
        render_filter(html, time_from, time_to, src_number, dst_number, limit, tab_show);

        if (timeFrom < timeTo && action.compare("Search")==0) {

            BDb db_calls(app().conf.db_calls);
            vector<Cdr> cdrs;
            if (getCdrUnfinished(&db_calls, time_from, time_to, src_number, dst_number, limit, tab_show, cdrs))
                render_cdr_table (time_from, time_to, html, cdrs);
        }

        html <<   "</html>";
    }

    void render_filter(std::stringstream &html, time_t &timeFrom, time_t &timeTo, string &src_number, string &dst_number, string &limit, string tab_show) {

        string date_from = str_date(timeFrom);
        string time_from = str_time(timeFrom);
        string time_to = str_time(timeTo);

        if (limit.size()==0)
            limit = string("100");
        else
            if (atoi(limit.c_str())>500)
                limit = string("500");

        string checked_unfin = tab_show.compare("unfin")==0? "checked":"";
        string checked_fin = tab_show.compare("fin")==0? "checked":"";
        string checked_all = tab_show.compare("all")==0? "checked":"";
        if (checked_unfin.size()==0 && checked_fin.size()==0 && checked_all.size()==0)
            checked_all = "checked";

//        string time_to = period_date(timeTo);

        html <<   "<head>\n"
//             <<   "    <meta charset=\"UTF-8\">\n"
             <<   "    <title>CDR Log</title>\n"
             <<   "    <link rel=\"stylesheet\" href=\"//code.jquery.com/ui/1.12.0/themes/base/jquery-ui.css\">\n"
             <<   "    <link rel=\"stylesheet\" href=\"/resources/demos/style.css\">\n"
             <<   "    <script src=\"https://code.jquery.com/jquery-1.12.4.js\"></script>\n"
             <<   "    <script src=\"https://code.jquery.com/ui/1.12.0/jquery-ui.js\"></script>\n"
             <<   "</head>\n"
             <<   "<body>\n"
             <<   "<form action=\"\" method=\"GET\">\n"
             <<   "    <label for=\"src_number\">src_number:</label> <input type=\"text\" name=\"src_number\" id=\"src_number\" value=\""+src_number+"\">\n"
             <<   "    <label for=\"dst_number\">dst_number:</label> <input type=\"text\" name=\"dst_number\" id=\"dst_number\" value=\""+dst_number+"\">\n"
             <<   "    <label for=\"date_from\">date_from:</label> <input type=\"text\" name=\"date_from\" id=\"date_from\" value=\"" << date_from << "\">\n"
             <<   "    <label for=\"time_from\">time_from:</label> <input type=\"text\" name=\"time_from\" id=\"time_from\" value=\"" << time_from << "\">\n"
             <<   "    <label for=\"time_to\">time_to:</label> <input type=\"text\" name=\"time_to\" id=\"time_to\" value=\"" << time_to << "\">\n"
             <<   "<br>"
             <<   "    <input type=\"submit\" value=\"Search\" name=\"do_search\">\n"
             <<   "    <input type=\"radio\" name=\"show\" value=\"all\" "+checked_all+"> show all\n"
             <<   "    <input type=\"radio\" name=\"show\" value=\"fin\" "+checked_fin+"> finished\n"
             <<   "    <input type=\"radio\" name=\"show\" value=\"unfin\" "+checked_unfin+"> unfinished\n"
             <<   "    <label for=\"limit\">limit:</label> <input type=\"text\" name=\"limit\" id=\"limit\" value=\""+limit+"\">\n"
             <<   "</form>\n"
             <<   "<script>\n"
             <<   "    $( function() {\n"
             <<   "        $( \"#date_from\" ).datepicker({ dateFormat: \"yy-mm-dd\" });\n"
             <<   "    } );\n"
             <<   "</script>\n"
             <<   "</body>\n";
    }

    void render_cdr_table (time_t timeFrom, time_t timeTo, std::stringstream &html, vector<Cdr> unfinishedCdrs) {

        html << "<table width=100% border=0 cellspacing=0>\n";
        html << "<tr>\n";
        html << "<td>CDR count: <b>" << unfinishedCdrs.size() << "</b>" << "</td>\n";
//        html << "<td>From: <b>" << sql_time(timeFrom) << "</b>" << "</td>\n";
//        html << "<td>To: <b>" << sql_time(timeTo) << "</b>" << "</td>\n";
        html << "</td>\n";
        html << "</tr>\n";
        html << "</table>\n";

        html << "<br/>\n";

        html << "<style>\n";
        html << ".unfinished { color: #1C1C1C; }\n";
        html << ".finished { color: #088A08; }\n";
        html << ".good { color: #04B404; }\n";
        html << ".bad { color: #ff0000; }\n";
        html << "</style>\n";

        html << "<table><tr><th>call_id</th><th>setup_time</th><th>connect_time</th><th>PDD</th><th>session_time</th><th>disconnect_cause</th>"
                "<th>src_number</th><th>dst_number</th><th>src_route</th><th>Our</th><th>dst_route</th><th>Our</th><th>releasing_party</th>"
                "<th>redirect_number</th>" //"<th>src_noa</th><th>dst_noa</th>"
                "<th>dst_replace</th></tr>";

        long long int current_call_id = 0;
        for (size_t i = 0; i < unfinishedCdrs.size(); i++) {

            Cdr cdr = unfinishedCdrs.at(i);
            bool normalDisconnectCause = std::set<int>{
                    CAUSE_NORMAL_CLEARING,   // 16
                    CAUSE_BUSY,              // 17
                    CAUSE_NO_REPONDING,      // 18
                    CAUSE_NO_ANSWER          // 19
            }
            .count(cdr.disconnect_cause) > 0;
            string row_class = normalDisconnectCause? string("finished"):string("unfinished");
            Trunk *src_trunk = repository.getTrunkByName(cdr.src_route);
            string src_trunk_type = "?";
            if (src_trunk!= nullptr)
                src_trunk_type = (src_trunk!= nullptr && src_trunk->our_trunk)? "+":"";

            Trunk *dst_trunk = repository.getTrunkByName(cdr.dst_route);
            string dst_trunk_type = "?";
            if (dst_trunk!= nullptr)
                dst_trunk_type = (dst_trunk!= nullptr && dst_trunk->our_trunk)? "+":"";

            html << "<tr>";
            if (cdr.call_id != current_call_id) {

                html << "<td class=" << row_class << ">" << cdr.call_id << "</td>";
                current_call_id = cdr.call_id;
            }
            else
                html << "<td> </td>";

            html << "<td class=" << row_class << ">" << (cdr.setup_time>0? string_time(cdr.setup_time):string(" ")) << "</td>";
            html << "<td class=" << row_class << ">" << (cdr.connect_time>0? string_time(cdr.connect_time):string(" ")) << "</td>";
            string pdd = " ";
            if ((cdr.setup_time>0) && (cdr.connect_time>0))
                pdd = lexical_cast<string>(cdr.connect_time-cdr.setup_time);

            html << "<td class=" << row_class << ">" << pdd << "</td>";
            html << "<td class=" << row_class << ">" << cdr.session_time << "</td>";
            html << "<td class=" << (normalDisconnectCause? "good":"bad") << ">" << cdr.disconnect_cause << "</td>";
            html << "<td class=" << row_class << ">" << cdr.src_number << "</td>";
            html << "<td class=" << row_class << ">" << cdr.dst_number << "</td>";
            html << "<td class=" << row_class << ">" << cdr.src_route << "</td>";
            html << "<td class=" << row_class << ">" << src_trunk_type << "</td>";
            html << "<td class=" << row_class << ">" << cdr.dst_route << "</td>";
            html << "<td class=" << row_class << ">" << dst_trunk_type << "</td>";
            html << "<td class=" << row_class << ">" << cdr.releasing_party << "</td>";
            html << "<td class=" << row_class << ">" << cdr.redirect_number << "</td>";
            /*if (cdr.src_noa != 0)
                html << "<td class=" << row_class << ">" << cdr.src_noa << "</td>";
            else
                html << "<td> </td>";

            if (cdr.dst_noa != 0)
                html << "<td class=" << row_class << ">" << cdr.dst_noa << "</td>";
            else
                html << "<td> </td>";*/

            html << "<td class=" << row_class << ">" << cdr.dst_replace << "</td>";
            html << "</tr>";

        }
        html << "</table>";
    }

    bool ready() {

        if (!repository.billingData->ready()) {
            return false;
        }
        if (!repository.prepare()) {
            return false;
        }
        return true;
    }
};
