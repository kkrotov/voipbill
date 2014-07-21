#include "ThreadSync.h"
#include "../classes/AppAuth.h"

typedef pair<string, string> fpair;

struct qsync {
    string label;
    string t_from;
    string t_to;
    list<fpair> fields;
    long long int count;

    string sfrom;
    string sto;

    void prepare() {
        sfrom = "";
        sto = "";
        list<fpair>::const_iterator i = fields.begin();
        int n = 1;
        while (i != fields.end()) {
            if (sfrom != "") {
                sfrom.append(",");
                sto.append(",");
            }
            const fpair *fp = &(*i);

            sfrom.append(fp->first);
            sto.append(fp->second);
            ++i;
            ++n;
        }

        count = 0;

    }

    void add_field(string f1, string f2) {

        fields.push_back(fpair(f1, f2));
    }

    void add_field(string f1) {
        add_field(f1, f1);
    }
};

void ThreadSync::run() {

    int newVersionId = getNewConfigVersionId();

    if (newVersionId > 0) {
        bool trans_calls = false;
        try {
            db_local.exec("BEGIN");
            trans_calls = true;


            for (list<qsync>::iterator s = syncs.begin(); s != syncs.end(); ++s) {

                string deleteQuery = "delete from " + s->t_to;

                string selectQuery = "select " + s->sfrom + " from " + s->t_from;
                if (s->t_from != "prefixlist_prefix" && s->t_from != "route_table_route") {
                    selectQuery += " where ";
                    selectQuery += (s->t_from == "config_version" ? "id" : "config_version_id");
                    selectQuery += " = " + lexical_cast<string>(newVersionId);
                }

                db_local.exec(deleteQuery);

                BDb::copy(s->t_to, "", s->sto, selectQuery, &db_main, &db_local);

                s->count += 1;

                boost::this_thread::interruption_point();
            }


            db_local.exec("COMMIT");
            trans_calls = false;
        } catch (Exception &e) {
            e.addTrace("ThreadSync::run");
            Log::exception(e);
            errors++;
            if (trans_calls) {
                try {
                    db_local.exec("ROLLBACK");
                } catch (...) {
                }
            }
        }
    }

}

int ThreadSync::getNewConfigVersionId() {
    int currentVersionId = 0;
    int newVersionId = 0;

    {
        auto res = db_main.query("select id from config_version where server_id = " + app().conf.str_server_id + " and status_id = 3");
        if (res.next()) {
            newVersionId = res.get_i(0);
        } else {
            return 0;
        }
    }

    {
        auto res = db_local.query("select id from config_version");
        if (res.next()) {
            currentVersionId = res.get_i(0);
        }
    }

    if (currentVersionId != newVersionId) {
        return newVersionId;
    } else {
        return 0;
    }
}

void ThreadSync::htmlfull(stringstream &html) {
    this->html(html);

    html << "Time loop: <b>" << t.sloop() << "</b><br/>\n";
    html << "Time full loop: <b>" << t.sfull() << "</b><br/>\n";
    html << "loops: <b>" << t.count << "</b><br/>\n";
    html << "<br/>\n";

    html << "Region: <b>" << app().conf.server_id << "</b><br/>\n";
    html << "Errors count: <b>" << errors << "</b><br/>\n";
    html << "<br/>\n";

    for (list<qsync>::const_iterator i = syncs.begin(); i != syncs.end(); ++i) {
        const qsync &s = *i;
        html << s.label << ": <b>" << s.count << "</b><br/>\n";
    }
}

ThreadSync::ThreadSync() {

    id = "sync";
    name = "Sync";

    errors = 0;
    db_main.setCS(app().conf.db_main);
    db_local.setCS(app().conf.db_local);

    qsync s1;
    s1.label = "config_version";
    s1.t_from = "config_version";
    s1.t_to = "config_version";
    s1.add_field("id");
    s1.add_field("name");
    s1.add_field("low_balance_outcome_id");
    s1.add_field("blocked_outcome_id");
    s1.add_field("calling_station_id_for_line_without_number");
    s1.add_field("route_table_id");
    s1.prepare();
    syncs.push_back(s1);

    qsync s2;
    s2.label = "airp";
    s2.t_from = "airp";
    s2.t_to = "airp";
    s2.add_field("id");
    s2.add_field("name");
    s2.prepare();
    syncs.push_back(s2);

    qsync s3;
    s3.label = "number";
    s3.t_from = "number";
    s3.t_to = "number";
    s3.add_field("id");
    s3.add_field("name");
    s3.add_field("prefixlist_ids");
    s3.prepare();
    syncs.push_back(s3);

    qsync s4;
    s4.label = "outcome";
    s4.t_from = "outcome";
    s4.t_to = "outcome";
    s4.add_field("id");
    s4.add_field("name");
    s4.add_field("calling_station_id");
    s4.add_field("called_station_id");
    s4.add_field("type_id");
    s4.add_field("route_case_id");
    s4.add_field("release_reason_id");
    s4.add_field("airp_id");
    s4.prepare();
    syncs.push_back(s4);

    qsync s5;
    s5.label = "prefixlist";
    s5.t_from = "prefixlist";
    s5.t_to = "prefixlist";
    s5.add_field("id");
    s5.add_field("name");
    s5.prepare();
    syncs.push_back(s5);

    qsync s6;
    s6.label = "prefixlist_prefix";
    s6.t_from = "prefixlist_prefix";
    s6.t_to = "prefixlist_prefix";
    s6.add_field("prefixlist_id");
    s6.add_field("prefix");
    s6.prepare();
    syncs.push_back(s6);

    qsync s7;
    s7.label = "release_reason";
    s7.t_from = "release_reason";
    s7.t_to = "release_reason";
    s7.add_field("id");
    s7.add_field("name");
    s7.prepare();
    syncs.push_back(s7);

    qsync s8;
    s8.label = "route_case";
    s8.t_from = "route_case";
    s8.t_to = "route_case";
    s8.add_field("id");
    s8.add_field("name");
    s8.prepare();
    syncs.push_back(s8);

    qsync s9;
    s9.label = "route_table";
    s9.t_from = "route_table";
    s9.t_to = "route_table";
    s9.add_field("id");
    s9.add_field("name");
    s9.prepare();
    syncs.push_back(s9);

    qsync s10;
    s10.label = "route_table_route";
    s10.t_from = "route_table_route";
    s10.t_to = "route_table_route";
    s10.add_field("route_table_id");
    s10.add_field("\"order\"");
    s10.add_field("a_number_id");
    s10.add_field("b_number_id");
    s10.add_field("outcome_id");
    s10.add_field("outcome_route_table_id");
    s10.prepare();
    syncs.push_back(s10);
}
