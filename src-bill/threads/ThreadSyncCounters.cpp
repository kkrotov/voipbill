#include "ThreadSyncCounters.h"
#include "../classes/AppBill.h"

ThreadSyncCounters::ThreadSyncCounters() {
    id = idName();
    name = "Sync Counters to central db";

    db_main.setCS(app().conf.db_main);

    billingData = DataBillingContainer::instance();
}

bool ThreadSyncCounters::ready() {
    return billingData->ready();
}

void ThreadSyncCounters::run() {

    save_client_counters();

}

void ThreadSyncCounters::save_client_counters() {

    string q;
    shared_ptr<ClientCounter> cl;

    auto clientCounter = billingData->clientCounter.get();

    for (auto it : clientCounter->counter) {
        ClientCounterObj &value = it.second;
        if (value.updated == 0) continue;

        if (q == "") {
            q.append("INSERT INTO billing.clients_counters(client_id,region_id,amount_month,amount_month_sum,amount_day,amount_day_sum,amount_sum,voip_auto_disabled,voip_auto_disabled_local)VALUES");
        } else {
            q.append(",");
        }
        q.append(string_fmt("(%d,'%d','%s','%f','%s','%f','%f',%s,%s)",
                it.first,
                app().conf.instance_id,
                string_date(value.amount_month).c_str(),
                value.sum_month,
                string_date(value.amount_day).c_str(),
                value.sum_day,
                value.sum,
                (value.disabled_global ? "true" : "false"),
                (value.disabled_local ? "true" : "false")));
        value.updated = 2;

    }

    if (q.length() > 0) {
        try {
            if (clientCounter->needTotalSync) {
                BDbTransaction trans(&db_main);

                db_main.exec("DELETE FROM billing.clients_counters WHERE region_id=" + app().conf.str_instance_id);
                db_main.exec(q);

                trans.commit();

                clientCounter->needTotalSync = false;
            } else {
                db_main.exec(q);
            }

            {
                for (auto it : clientCounter->counter) {
                    if (it.second.updated == 2) {
                        it.second.updated = 0;
                    }
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

}

