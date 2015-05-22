#include "ThreadSyncCounters.h"
#include "../classes/AppBill.h"

ThreadSyncCounters::ThreadSyncCounters() {
    id = idName();
    name = "Sync Counters to central db";

    db_main.setCS(app().conf.db_main);

    billingData = DataBillingContainer::instance();

    last_sync_count = 0;
    total_sync_count = 0;
}

bool ThreadSyncCounters::ready() {
    return billingData->ready();
}

void ThreadSyncCounters::run() {

    unique_lock<mutex> lock(billingData->syncCountersCentralLock, try_to_lock);
    if (!lock.owns_lock()) {
        return;
    }

    save_client_counters();

}

void ThreadSyncCounters::save_client_counters() {

    string q;

    auto clientCounter = billingData->clientCounter.get();

    int sync_count = 0;

    for (auto it : clientCounter->counter) {
        ClientCounterObj &value = clientCounter->counter[it.second.client_id];
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
        sync_count += 1;
    }

    if (sync_count > 0) {
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
                    ClientCounterObj &value = clientCounter->counter[it.second.client_id];
                    if (value.updated == 2) {
                        value.updated = 0;
                    }
                }
            }
        } catch (Exception &e) {
            e.addTrace("ThreadSyncCounters::save_client_counters:");
            throw e;
        }
    }

    last_sync_count = sync_count;
    total_sync_count += sync_count;
}

void ThreadSyncCounters::htmlfull(stringstream &html) {
    this->html(html);

    html << "Last sync count: " << last_sync_count << "<br/>\n";
    html << "Last sync count: " << total_sync_count << "<br/>\n";
}

bool ThreadSyncCounters::hasFullHtml() {
    return true;
}
