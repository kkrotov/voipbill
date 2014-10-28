#include "ThreadSync.h"
#include "../classes/AppBill.h"

typedef pair<string, string> fpair;

struct qsync {
    bool full;
    string label;
    string t_from;
    string t_to;
    list<fpair> fields;
    long long int count;

    string ifs;
    string sel;
    string ins;
    string del;
    string fix;
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
        if (full) {
            ifs = "select rnd from billing.sync z where z.region='" + app().conf.str_instance_id + "' and z.table='" + label + "' and z.obj=0";
            del = "delete from " + t_to;
            fix = "delete from billing.sync z where z.region='" + app().conf.str_instance_id + "' and z.table='" + label + "' and z.rnd<=%s ";
        } else {
            sel = "select " + sfrom + ", z.rnd " \
                    "from billing.sync z " \
                    "left join " + t_from + " x on z.obj=x.id " \
                    "where z.region='" + app().conf.str_instance_id + "' and z.table='" + label + "' " \
                    "order by z.rnd ";
            //"limit 100";
            ins = "insert into " + t_to + "(" + sto + ") VALUES ";
            del = "delete from " + t_to + " where id in (%s)";
            fix = "delete from billing.sync z where z.region='" + app().conf.str_instance_id + "' and z.table='" + label + "' and z.rnd<=%s ";
        }
        count = 0;

    }

    void add_field(string f1, string f2) {

        if (f1.find(".") == string::npos) f1 = "x." + f1;
        fields.push_back(fpair(f1, f2));
    }

    void add_field(string f1) {
        add_field(f1, f1);
    }

    void add_autofield() {
        fields.push_front(fpair("z.obj", "id"));
    }
};

bool ThreadSync::sync_full(qsync &s) {
    try {
        //ifs
        BDbResult res = db_main.query(s.ifs);
        if (res.size() == 0) {
            return true;
        }
        res.next();
        string rnd = res.get_s(0);

        BDbTransaction trans(&db_calls);

        db_calls.exec(s.del);
        BDb::copy(s.t_to, s.t_from, s.sto, "", &db_main, &db_calls);

        trans.commit();

        db_main.exec(string_fmt(s.fix, rnd.c_str()));
        s.count += 1000000;
    } catch (Exception &e) {
        e.addTrace("ThreadSync::sync_full");
        Log::exception(e);
        errors++;
        return false;
    }
    return true;
}

bool ThreadSync::sync_notfull(qsync &s) {
    int size;
    try {
        BDbResult res = db_main.query(s.sel);
        if (res.size() == 0) {
            return true;
        }
        string rnd = "0";
        string qins = "";
        string qdel = "";
        while (res.next()) {
            rnd = res.get_s(s.fields.size());

            string id(res.get(0));
            if (strlen(res.get(1)) > 0) {
                if (qins != "") qins.append(",");
                string qi = "(";
                for (size_t col = 0; col < s.fields.size(); col++) {
                    if (col > 0) qi.append(",");
                    char * val = res.get(col);
                    if (strlen(val) > 0)
                        qi.append("'").append(val).append("'");
                    else
                        qi.append("NULL");
                }
                qi.append(")");
                qins.append(qi);
            } else {
                if (qdel != "") qdel.append(",");
                qdel.append("'" + id + "'");
            }
        }
        size = res.size();
        if (rnd.compare("0") != 0) {
            if (qins != "") db_calls.exec(s.ins + qins);
            if (qdel != "") db_calls.exec(string_fmt(s.del, qdel.c_str()));
            db_main.exec(string_fmt(s.fix, rnd.c_str()));
            s.count += size;
        }

    } catch (Exception &e) {
        e.addTrace("ThreadSync::sync_notfull");
        Log::exception(e);
        errors++;
        return false;
    }
    return true;
}

bool ThreadSync::prepare() {
    if (!app().init_sync_done) {
        Log::info("Sync...");
        if (!this->do_sync()) {
            return false;
        }
        app().init_sync_done = true;
    }
    return true;
}

void ThreadSync::run() {

    for (list<qsync>::iterator s = syncs.begin(); s != syncs.end(); ++s) {
        if ((*s).full) {
            sync_full(*s);
        } else {
            sync_notfull(*s);
        }

        boost::this_thread::interruption_point();
    }

}

void ThreadSync::htmlfull(stringstream &html) {
    this->html(html);

    html << "Time loop: <b>" << t.sloop() << "</b><br/>\n";
    html << "Time full loop: <b>" << t.sfull() << "</b><br/>\n";
    html << "loops: <b>" << t.count << "</b><br/>\n";
    html << "<br/>\n";

    html << "Region: <b>" << app().conf.instance_id << "</b><br/>\n";
    html << "Errors count: <b>" << errors << "</b><br/>\n";
    html << "<br/>\n";

    for (list<qsync>::const_iterator i = syncs.begin(); i != syncs.end(); ++i) {
        const qsync &s = *i;
        html << s.label << ": <b>" << s.count << "</b><br/>\n";
    }
}

bool ThreadSync::do_sync() {
    for (list<qsync>::iterator s = syncs.begin(); s != syncs.end(); ++s) {
        if ((*s).full) {
            if (sync_full(*s) == false) return false;
        } else {
            if (sync_notfull(*s) == false) return false;
        }
    }
    return true;
}

ThreadSync::ThreadSync() {
    id = idName();
    name = "Sync";

    errors = 0;
    db_main.setCS(app().conf.db_main);
    db_calls.setCS(app().conf.db_calls);

    qsync s1;
    s1.label = "clients";
    s1.full = false;
    s1.t_from = "billing.clients";
    s1.t_to = "billing.clients";
    s1.add_autofield();
    s1.add_field("voip_limit_month");
    s1.add_field("voip_limit_day");
    s1.add_field("voip_disabled");
    s1.add_field("balance");
    s1.add_field("credit");
    s1.add_field("amount_date");
    s1.add_field("last_payed_month");
    s1.prepare();
    syncs.push_back(s1);

    qsync s2;
    s2.label = "usage";
    s2.full = false;
    s2.t_from = "billing.usage";
    s2.t_to = "billing.usage";
    s2.add_autofield();
    s2.add_field("client_id");
    s2.add_field("phone_num");
    s2.add_field("actual_from");
    s2.add_field("actual_to");
    s2.add_field("no_of_lines");
    s2.add_field("region");
    s2.prepare();
    syncs.push_back(s2);

    qsync s3;
    s3.label = "tarif_log";
    s3.full = false;
    s3.t_from = "billing.tarif_change_log";
    s3.t_to = "billing.tarif_change_log";
    s3.add_autofield();
    s3.add_field("usage_id");
    s3.add_field("ts");
    s3.add_field("date_activation");
    s3.add_field("tarif_id_local");
    s3.add_field("tarif_id_local_mob");
    s3.add_field("tarif_id_russia");
    s3.add_field("tarif_id_russia_mob");
    s3.add_field("tarif_id_intern");
    s3.add_field("tarif_id_sng");
    s3.prepare();
    syncs.push_back(s3);

    qsync s7;
    s7.label = "tarif";
    s7.full = true;
    s7.t_from = "billing.tarif";
    s7.t_to = "billing.tarif";
    s7.add_field("id");
    s7.add_field("freemin");
    s7.add_field("freemin_for_number");
    s7.add_field("pricelist_id");
    s7.add_field("paid_redirect");
    s7.add_field("tariffication_by_minutes");
    s7.add_field("tariffication_full_first_minute");
    s7.add_field("tariffication_free_first_seconds");
    s7.prepare();
    syncs.push_back(s7);

    qsync s8;
    s8.label = "defs";
    s8.full = true;
    s8.t_from = "billing.defs";
    s8.t_to = "billing.defs";
    s8.add_field("ndef");
    s8.add_field("pricelist_id");
    s8.add_field("date_from");
    s8.add_field("deleted");
    s8.add_field("date_to");
    s8.add_field("price");
    s8.prepare();
    syncs.push_back(s8);

    qsync s9;
    s9.label = "geo_prefix";
    s9.full = true;
    s9.t_from = "geo.prefix";
    s9.t_to = "geo.prefix";
    s9.add_field("prefix");
    s9.add_field("geo_id");
    s9.add_field("operator_id");
    s9.add_field("mob");
    s9.add_field("dest");
    s9.add_field("region");
    s9.prepare();
    syncs.push_back(s9);

    qsync s10;
    s10.label = "instance";
    s10.full = true;
    s10.t_from = "billing.instance_settings";
    s10.t_to = "billing.instance_settings";
    s10.add_field("id");
    s10.add_field("region_id");
    s10.add_field("city_geo_id");
    s10.prepare();
    syncs.push_back(s10);

    qsync s11;
    s11.label = "operator";
    s11.full = true;
    s11.t_from = "voip.operator";
    s11.t_to = "billing.operator";
    s11.add_field("region");
    s11.add_field("id");
    s11.add_field("pricelist_id");
    s11.add_field("term_in_cost");
    s11.add_field("local_network_pricelist_id");
    s11.add_field("client_7800_pricelist_id");
    s11.add_field("operator_7800_pricelist_id");
    s11.prepare();
    syncs.push_back(s11);

    qsync s12;
    s12.label = "pricelist";
    s12.full = true;
    s12.t_from = "voip.pricelist";
    s12.t_to = "billing.pricelist";
    s12.add_field("id");
    s12.add_field("region");
    s12.add_field("operator_id");
    s12.add_field("tariffication_by_minutes");
    s12.add_field("tariffication_full_first_minute");
    s12.prepare();
    syncs.push_back(s12);

    qsync s13;
    s13.label = "network_prefix";
    s13.full = true;
    s13.t_from = "billing.network_prefix";
    s13.t_to = "billing.network_prefix";
    s13.add_field("prefix");
    s13.add_field("instance_id");
    s13.add_field("operator_id");
    s13.add_field("date_from");
    s13.add_field("deleted");
    s13.add_field("date_to");
    s13.add_field("network_type_id");
    s13.prepare();
    syncs.push_back(s13);

}
