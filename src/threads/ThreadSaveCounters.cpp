#include "ThreadSaveCounters.h"

#include "../classes/KillCalls.h"

ThreadSaveCounters::ThreadSaveCounters() {
    id = "savecounters";
    name = "Save counters";

    loader = DataLoader::instance();

    db_main.setCS(app.conf.db_main);
    db_main.needAdvisoryLock(app.conf.region_id);

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


    while (true) {

        t.start();

        save_client_counters();

        save_calls();

        t.stop();

        ssleep(1);

    }
}

bool ThreadSaveCounters::save_client_counters(bool clear) {

    loader->counter_rwlock.lock();

    shared_ptr<ClientCounter> cl = loader->counter_client;
    if (cl == 0) {
        loader->counter_rwlock.unlock();
        return false;
    }

    string q;
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
                app.conf.region_id,
                string_date(value->amount_month).c_str(),
                value->sum_month,
                string_date(value->amount_day).c_str(),
                value->sum_day,
                value->sum,
                (value->disabled_global ? "true" : "false"),
                (value->disabled_local ? "true" : "false")));
        value->updated = 2;

    }
    loader->counter_rwlock.unlock();

    if (q.length() > 0) {
        try {
            if (clear) {
                db_main.exec("BEGIN");
                db_main.exec("DELETE FROM billing.clients_counters WHERE region_id=" + app.conf.str_region_id);
            }
            db_main.exec(q);
            if (clear) {
                db_main.exec("COMMIT");
            }
            loader->counter_rwlock.lock(); //.lockForWrite();
            for (map<int, ClientCounterObj>::iterator i = cl->counter.begin(); i != cl->counter.end(); ++i) {
                if (i->second.updated == 2)
                    i->second.updated = 0;
            }
            loader->counter_rwlock.unlock();
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

    long long int last_id;

    try {
        BDbResult res = db_main.query("select max(id) from billing.calls_" + app.conf.str_region_id);
        res.next();
        last_id = res.get_ll(0);
    } catch (Exception &e) {
        e.addTrace("ThreadSaveCounters::save_calls::get_last_id");
        Log::exception(e);
        return false;
    }

    try {
        BDb::copy("billing.calls_" + app.conf.str_region_id,
                "",
                "       id, time, direction_out, usage_num, phone_num, len, usage_id, pricelist_mcn_id, operator_id, free_min_groups_id, dest, mob, redirect, month, day, amount, amount_op, client_id, region, geo_id, pricelist_op_id, price, price_op, srv_region_id",
                "select id, time, direction_out, usage_num, phone_num, len, usage_id, pricelist_mcn_id, operator_id, free_min_groups_id, dest, mob, redirect, month, day, amount, amount_op, client_id, region, geo_id, pricelist_op_id, price, price_op, " + app.conf.str_region_id + "::smallint from billing.calls where id>" + lexical_cast<string>(last_id) + " order by id limit 100000",
                &db_calls, &db_main);

    } catch (Exception &e) {
        e.addTrace("ThreadSaveCounters::save_calls::copy(last_id:" + lexical_cast<string>(last_id) + ")");
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

