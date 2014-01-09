#include "ThreadSaveCounters.h"
#include "../classes/App.h"
#include "../classes/KillCalls.h"

ThreadSaveCounters::ThreadSaveCounters() {
    id = "savecounters";
    name = "Save counters";

    loader = DataLoader::instance();

    db_main.setCS(app.conf.db_main);
    db_main.needAdvisoryLock(app.conf.instance_id);

    db_calls.setCS(app.conf.db_calls);
}

bool ThreadSaveCounters::ready() {
    return app.init_sync_done &&
            app.init_load_data_done &&
            app.init_load_counters_done &&
            app.init_bill_runtime_started;
}

bool ThreadSaveCounters::prepare() {

    if (!save_client_counters(true)) {
        return false;
    }

    return true;
}

void ThreadSaveCounters::run() {

    save_client_counters();

    boost::this_thread::interruption_point();

    save_calls();

}

bool ThreadSaveCounters::save_client_counters(bool clear) {

    string q;
    shared_ptr<ClientCounter> cl;

    {
        lock_guard<mutex> lock(loader->counter_rwlock);

        cl = loader->counter_client;
        if (cl == 0) return false;

        for (map<int, ClientCounterObj>::iterator i = cl->counter.begin(); i != cl->counter.end(); ++i) {
            ClientCounterObj *value = (ClientCounterObj*) & i->second;
            if (value->updated == 0) continue;

            if (q == "") {
                q.append("INSERT INTO billing.clients_counters(client_id,region_id,amount_month,amount_month_sum,amount_day,amount_day_sum,amount_sum,voip_auto_disabled,voip_auto_disabled_local)VALUES");
            } else {
                q.append(",");
            }
            q.append(string_fmt("(%d,'%d','%s',%d,'%s',%d,%d,%s,%s)",
                    i->first,
                    app.conf.instance_id,
                    string_date(value->amount_month).c_str(),
                    value->sum_month,
                    string_date(value->amount_day).c_str(),
                    value->sum_day,
                    value->sum,
                    (value->disabled_global ? "true" : "false"),
                    (value->disabled_local ? "true" : "false")));
            value->updated = 2;

        }
    }

    if (q.length() > 0) {
        try {
            if (clear) {
                db_main.exec("BEGIN");
                db_main.exec("DELETE FROM billing.clients_counters WHERE region_id=" + app.conf.str_instance_id);
            }

            db_main.exec(q);

            if (clear) {
                db_main.exec("COMMIT");
            }

            {
                lock_guard<mutex> lock(loader->counter_rwlock);
                for (map<int, ClientCounterObj>::iterator i = cl->counter.begin(); i != cl->counter.end(); ++i) {
                    if (i->second.updated == 2)
                        i->second.updated = 0;
                }
            }
        } catch (Exception &e) {
            e.addTrace("ThreadSaveCounters::save_client_counters:");
            Log::exception(e);
            if (clear) {
                try {
                    db_main.exec("ROLLBACK");
                } catch (...) {
                }
            }
            return false;
        }
    }

    return true;
}

bool ThreadSaveCounters::save_calls() {

    long long int main_last_id;
    try {
        BDbResult res = db_main.query("select max(id) from calls.calls_" + app.conf.str_instance_id);
        main_last_id = res.next() ? res.get_ll(0) : 0;
    } catch (Exception &e) {
        e.addTrace("ThreadSaveCounters::save_calls::get_main_last_id");
        Log::exception(e);
        return false;
    }

    string local_sync_month;
    try {
        BDbResult res = db_calls.query("select month from calls.calls where id>" + lexical_cast<string>(main_last_id) + " order by id");
        if (!res.next())
            return true;
        local_sync_month += res.get_s(0).substr(0, 4);
        local_sync_month += res.get_s(0).substr(5, 2);
    } catch (Exception &e) {
        e.addTrace("ThreadSaveCounters::save_calls::get_local_sync_month");
        Log::exception(e);
        return false;
    }

    try {
        BDb::copy("calls.calls_" + app.conf.str_instance_id + "_" + local_sync_month,
                "",
                "       id, time, direction_out, usage_num, phone_num, len, usage_id, pricelist_mcn_id, operator_id, free_min_groups_id, dest, mob, redirect, month, day, amount, amount_op, client_id, region, geo_id, pricelist_op_id, price, price_op, len_mcn, len_op, prefix_geo, prefix_mcn, prefix_op, srv_region_id",
                "select id, time, direction_out, usage_num, phone_num, len, usage_id, pricelist_mcn_id, operator_id, free_min_groups_id, dest, mob, redirect, month, day, amount, amount_op, client_id, region, geo_id, pricelist_op_id, price, price_op, len_mcn, len_op, prefix_geo, prefix_mcn, prefix_op, " + app.conf.str_instance_id + "::smallint from calls.calls_" + local_sync_month + " where id>" + lexical_cast<string>(main_last_id) + " order by id limit 100000",
                &db_calls, &db_main);

    } catch (Exception &e) {
        e.addTrace("ThreadSaveCounters::save_calls::copy(main_last_id:" + lexical_cast<string>(main_last_id) + ")");
        Log::exception(e);
        return false;
    }
    return true;
}

void ThreadSaveCounters::htmlfull(stringstream &html) {
    this->html(html);

    html << "Time loop: <b>" << t.sloop() << "</b><br/>\n";
    html << "Time full loop: <b>" << t.sfull() << "</b><br/>\n";
    html << "loops: <b>" << t.count << "</b><br/>\n";
    html << "<br/>\n";

}

