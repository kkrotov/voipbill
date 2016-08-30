#pragma once

#include "BasePage.h"
#include "../classes/Repository.h"
#include "../classes/DbException.h"

class PageCdrUnfinished : public BasePage {
    Spinlock lock;

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

    string from_date (time_t time) {

        char buff[128];
        struct tm * timeinfo = gmtime(&time);
        strftime(buff, sizeof(buff), "%Y-%m-%d", timeinfo);
        return string(buff);
    }

    time_t uri_time (const char*uriTime) {

        if (uriTime == nullptr)
            return time(NULL);

        // from=20160828085734&to=20160829085734
        // date_from=2016-08-29&date_to=2016-09-01&do_search=Go
        struct tm uri_tm = {0};
//       if (sscanf(uriTime, "%4d%2d%2d%2d%2d%2d", &uri_tm.tm_year, &uri_tm.tm_mon, &uri_tm.tm_mday, &uri_tm.tm_hour, &uri_tm.tm_min, &uri_tm.tm_sec) <= 0)
        if (sscanf(uriTime, "%d-%d-%d", &uri_tm.tm_year, &uri_tm.tm_mon, &uri_tm.tm_mday) <= 0)
            return time(NULL);

        uri_tm.tm_year -= 1900;
        uri_tm.tm_mon -= 1;
        return timegm(&uri_tm);
    }

    bool getCdrUnfinished(BDb * db_calls, time_t timeFrom, time_t timeTo, vector<Cdr> &cdrUnfinished) {

        if (timeFrom > timeTo)
            return false;

        string cdr_unfinished = string("calls_cdr.cdr_unfinished_")+time_suffix(timeFrom);
        string calls_cdr = string("calls_cdr.cdr_")+time_suffix(timeFrom);
        string query =
                "select "+
                    cdr_unfinished+".call_id, "+                    // 0
                    cdr_unfinished+".setup_time, "+                 // 1
                    cdr_unfinished+".src_number, "+                 // 2
                    cdr_unfinished+".dst_number, "+                 // 3
                    cdr_unfinished+".src_route, "+                  // 4
                    cdr_unfinished+".dst_route, "+                  // 5
                    cdr_unfinished+".releasing_party, "+            // 6
                    cdr_unfinished+".release_timestamp, "+          // 7
                    cdr_unfinished+".disconnect_cause, "+           // 8
//                    calls_cdr+".nas_ip, "+
                    calls_cdr+".src_number, "+                      // 9
                    calls_cdr+".dst_number, "+                      // 10
                    calls_cdr+".redirect_number, "+                 // 11
                    calls_cdr+".connect_time, "+                    // 12
                    calls_cdr+".disconnect_time, "+                 // 13
                    calls_cdr+".session_time, "+                    // 14
                    calls_cdr+".disconnect_cause, "+                // 15
                    calls_cdr+".src_route, "+                       // 16
                    calls_cdr+".dst_route, "+                       // 17
                    calls_cdr+".src_noa, "+                         // 18
                    calls_cdr+".dst_noa, "+                         // 19
                    calls_cdr+".dst_replace "                       // 20
                "from "+cdr_unfinished+" left join "+calls_cdr+" using(hash) "
                "where "+cdr_unfinished+".setup_time>='"+sql_time(timeFrom)+"' and "+cdr_unfinished+".setup_time<='"+sql_time(timeTo)+"' "
                "order by "+cdr_unfinished+".call_id desc, "+cdr_unfinished+".setup_time desc";

        try {

            BDbResult res = db_calls->query(query);
            if (res.size() < 0)
                return false;

            lock_guard<Spinlock> guard(lock);

            long long int current_call_id = 0;
            while (res.next()) {

                long long int call_id = res.get_ll(0);
                time_t setup_time = parseDateTime(res.get(1));
                char src_number_unfinished[33];
                strcpy((char *) &src_number_unfinished, res.get(2));
                char dst_number_unfinished[33];
                strcpy((char *) &dst_number_unfinished, res.get(3));
                char src_route_unfinished[33];
                strcpy((char *) &src_route_unfinished, res.get(4));
                char dst_route_unfinished[33];
                strcpy((char *) &dst_route_unfinished, res.get(5));
                char releasing_party_unfinished[32];
                strncpy((char*) &releasing_party_unfinished, res.get(6), sizeof(releasing_party_unfinished));
                time_t releasing_time = parseDateTime(res.get(7));
                int disconnect_cause_unfinished = res.get_i(8);

                char src_number[33];
                strcpy((char *) &src_number, res.get(9));
                char dst_number[33];
                strcpy((char *) &dst_number, res.get(10));
                char redirect_number[33];
                strcpy((char *) &redirect_number, res.get(11));
                time_t connect_time = parseDateTime(res.get(12));
                time_t disconnect_time = parseDateTime(res.get(13));
                time_t session_time = res.get_i(14);
                int disconnect_cause_finished = res.get_i(15);
                char src_route[33];
                strcpy((char *) &src_route, res.get(16));
                char dst_route_finished[33];
                strcpy((char *) &dst_route_finished, res.get(17));
                int src_noa = res.get_i(18);
                int dst_noa = res.get_i(19);
                char dst_replace[33];
                strcpy((char*)&dst_replace, res.get(20));

                if (call_id != current_call_id) {

                    if (src_number[0] != '\0') {

                        Cdr cdr;
                        cdr.call_id = call_id;
                        strcpy((char *) &cdr.dst_route, dst_route_finished);
                        cdr.disconnect_cause = disconnect_cause_finished;
                        strcpy((char *) &cdr.src_number, src_number);
                        strcpy((char *) &cdr.dst_number, dst_number);
                        strcpy((char *) &cdr.redirect_number, redirect_number);
                        cdr.connect_time = connect_time;
                        cdr.session_time = session_time;
                        strcpy((char *) &cdr.src_route, src_route);
                        cdr.src_noa = src_noa;
                        cdr.dst_noa = dst_noa;
                        strcpy((char *) &cdr.dst_replace, dst_replace);

                        cdrUnfinished.push_back(cdr);
                    }
                    current_call_id = call_id;
                }
                Cdr cdr;
                cdr.call_id = call_id;
                cdr.connect_time = setup_time;
                strcpy((char *) &cdr.src_number, src_number_unfinished);
                strcpy((char *) &cdr.dst_number, dst_number_unfinished);
                strcpy((char *) &cdr.src_route, src_route_unfinished);
                strcpy((char *) &cdr.dst_route, dst_route_unfinished);
                strncpy((char *) &cdr.releasing_party, releasing_party_unfinished, sizeof(cdr.releasing_party));
                cdr.disconnect_cause = disconnect_cause_unfinished;
                cdr.session_time = releasing_time - setup_time;

                cdrUnfinished.push_back(cdr);
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

        html << "<!DOCTYPE html>\n" << "<html lang=\"en\">\n";
        renderHeader(html);

        time_t timeFrom = uri_time(parameters["date_from"].c_str());
        time_t timeTo = timeFrom+3600*24; //uri_time(parameters["date_to"].c_str());
//        string src_number_pattern = parameters["src_number"];
//        string dst_number_pattern = parameters["dst_number"];
        string action = parameters["do_search"];
        render_time_from_to(html, timeFrom, timeTo);

        if (timeFrom < timeTo && action.compare("Search")==0)
            render_cdr_table (timeFrom, timeTo, html, parameters);

        html <<   "</html>";
    }

    void render_time_from_to(std::stringstream &html, time_t &timeFrom, time_t &timeTo) {

        string time_from = from_date(timeFrom);
//        string time_to = period_date(timeTo);
        html <<   "<head>\n"
//             <<   "    <meta charset=\"UTF-8\">\n"
             <<   "    <title>CDR Unfinished</title>\n"
             <<   "    <link rel=\"stylesheet\" href=\"//code.jquery.com/ui/1.12.0/themes/base/jquery-ui.css\">\n"
             <<   "    <link rel=\"stylesheet\" href=\"/resources/demos/style.css\">\n"
             <<   "    <script src=\"https://code.jquery.com/jquery-1.12.4.js\"></script>\n"
             <<   "    <script src=\"https://code.jquery.com/ui/1.12.0/jquery-ui.js\"></script>\n"
             <<   "</head>\n"
             <<   "<body>\n"
             <<   "<form action=\"\" method=\"GET\">\n"
//             <<   "    <label for=\"src_number\">src_number:</label> <input type=\"text\" name=\"src_number\" id=\"src_number\" value=\"\">\n"
//             <<   "    <label for=\"dst_number\">dst_number:</label> <input type=\"text\" name=\"dst_number\" id=\"dst_number\" value=\"\">\n"
             <<   "    <label for=\"date_from\">date_from:</label> <input type=\"text\" name=\"date_from\" id=\"date_from\" value=\"" << time_from << "\">\n"
//             <<   "    <label for=\"date_to\">date_to: <input type=\"text\" name=\"date_to\" id=\"date_to\" value=\"" << time_to << "\">\n"
             <<   "    <input type=\"submit\" value=\"Search\" name=\"do_search\">\n"
             <<   "</form>\n"
             <<   "<script>\n"
             <<   "    $( function() {\n"
             <<   "        $( \"#date_from\" ).datepicker({ dateFormat: \"yy-mm-dd\" });\n"
             <<   "    } );\n"
             <<   "</script>\n"
             <<   "</body>\n";
    }

    void render_cdr_table (time_t timeFrom, time_t timeTo, std::stringstream &html, map<string, string> &parameters) {

        BDb db_calls(app().conf.db_calls);
        vector<Cdr> unfinishedCdrs;
        if (!getCdrUnfinished(&db_calls, timeFrom, timeTo, unfinishedCdrs))
            return;

        html << "<table width=100% border=0 cellspacing=0>\n";
        html << "<tr>\n";
        html << "<td>Unfinished CDRs: <b>" << unfinishedCdrs.size() << "</b>" << "</td>\n";
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

        html << "<table><tr><th>call_id</th><th>connect_time</th><th>session_time</th><th>disconnect_cause</th>"
                "<th>src_number</th><th>dst_number</th><th>src_route</th><th>dst_route</th><th>releasing_party</th>"
                "<th>redirect_number</th><th>src_noa</th><th>dst_noa</th><th>dst_replace</th></tr>";

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

            html << "<tr>";
            if (cdr.call_id != current_call_id) {

                html << "<td class=" << row_class << ">" << cdr.call_id << "</td>";
                current_call_id = cdr.call_id;
            }
            else
                html << "<td> </td>";

            html << "<td class=" << row_class << ">" << (cdr.connect_time>0? string_time(cdr.connect_time):string(" ")) << "</td>";
            html << "<td class=" << row_class << ">" << cdr.session_time << "</td>";
            html << "<td class=" << (normalDisconnectCause? "good":"bad") << ">" << cdr.disconnect_cause << "</td>";
            html << "<td class=" << row_class << ">" << cdr.src_number << "</td>";
            html << "<td class=" << row_class << ">" << cdr.dst_number << "</td>";
            html << "<td class=" << row_class << ">" << cdr.src_route << "</td>";
            html << "<td class=" << row_class << ">" << cdr.dst_route << "</td>";
            html << "<td class=" << row_class << ">" << cdr.releasing_party << "</td>";
            html << "<td class=" << row_class << ">" << cdr.redirect_number << "</td>";
            if (cdr.src_noa != 0)
                html << "<td class=" << row_class << ">" << cdr.src_noa << "</td>";
            else
                html << "<td> </td>";

            if (cdr.dst_noa != 0)
                html << "<td class=" << row_class << ">" << cdr.dst_noa << "</td>";
            else
                html << "<td> </td>";

            html << "<td class=" << row_class << ">" << cdr.dst_replace << "</td>";
            html << "</tr>";

        }
        html << "</table>";
    }
};
