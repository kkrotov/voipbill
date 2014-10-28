#include "ThreadSyncCounters.h"
#include "../classes/AppBill.h"

ThreadSyncCounters::ThreadSyncCounters() {
    id = idName();
    name = "Sync Counters to central db";

    db_main.setCS(app().conf.db_main);
    db_main.needAdvisoryLock(app().conf.instance_id);

    loader = DataLoader::instance();
}

bool ThreadSyncCounters::ready() {
    return app().init_sync_done &&
            app().init_load_data_done &&
            app().init_load_counters_done &&
            app().init_bill_runtime_started;
}

bool ThreadSyncCounters::prepare() {

    save_client_counters(true);

    return true;
}

void ThreadSyncCounters::run() {

    save_client_counters();

}

void ThreadSyncCounters::save_client_counters(bool clear) {

    string q;
    shared_ptr<ClientCounter> cl;

    {
        lock_guard<mutex> lock(loader->counter_rwlock);

        cl = loader->counter_client;
        if (cl == 0) {
            Exception e("loader->counter_client is null", "ThreadSaveCounters::save_client_counters:");
            throw e;
        }

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
                    app().conf.instance_id,
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
                BDbTransaction trans(&db_main);

                db_main.exec("DELETE FROM billing.clients_counters WHERE region_id=" + app().conf.str_instance_id);
                db_main.exec(q);

                trans.commit();
            } else {
                db_main.exec(q);
            }

            {
                lock_guard<mutex> lock(loader->counter_rwlock);
                for (map<int, ClientCounterObj>::iterator i = cl->counter.begin(); i != cl->counter.end(); ++i) {
                    if (i->second.updated == 2)
                        i->second.updated = 0;
                }
            }
        } catch (Exception &e) {
            e.addTrace("ThreadSyncCounters::save_client_counters:");
            throw e;
        }
    }
}

void ThreadSyncCounters::htmlfull(stringstream &html) {
    this->html(html);

    html << "Time loop: <b>" << t.sloop() << "</b><br/>\n";
    html << "Time full loop: <b>" << t.sfull() << "</b><br/>\n";
    html << "loops: <b>" << t.count << "</b><br/>\n";
    html << "<br/>\n";

}

