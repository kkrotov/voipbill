#include "ThreadSyncCounters.h"
#include "../classes/AppBill.h"
#include "../data/ClientCounterData.h"

ThreadSyncCounters::ThreadSyncCounters() {
    id = idName();
    name = "Sync Counters to central db";

    db_main.setCS(app().conf.db_main);

    last_sync_count = 0;
    total_sync_count = 0;
}

bool ThreadSyncCounters::ready() {
    return repository.billingData->ready();
}

void ThreadSyncCounters::run() {

    unique_lock<mutex> lock(repository.billingData->syncCountersCentralLock, try_to_lock);
    if (!lock.owns_lock()) {
        return;
    }

    return;

    save_client_counters();
}

void ThreadSyncCounters::save_client_counters() {

    string q;

    ClientCounterData clientCounterData;

    auto clientCounter = clientCounterData.get();

    vector<int> changes;
    unsigned long long int marker;
    bool needTotalSync;

    clientCounter->getChanges(marker, changes, needTotalSync);

    int sync_count = 0;

    for (int account_id : changes) {
        ClientCounterObj value = clientCounter->get(account_id);

        if (sync_count == 0) {
            q.append(
                    "INSERT INTO billing.clients_counters(client_id,region_id,amount_month,amount_month_sum,amount_day,amount_day_sum,amount_date,amount_sum)VALUES");
        } else {
            q.append(",");
        }
        q.append(string_fmt("(%d,'%d','%s','%f','%s','%f','%s','%f')",
                            account_id,
                            app().conf.instance_id,
                            string_date(value.amount_month).c_str(),
                            value.sum_month,
                            string_date(value.amount_day).c_str(),
                            value.sum_day,
                            string_time(value.amount_date).c_str(),
                            value.sum));
        sync_count += 1;
    }

    if (sync_count > 0) {
        try {
            if (needTotalSync) {
                BDbTransaction trans(&db_main);

                db_main.exec("DELETE FROM billing.clients_counters WHERE region_id=" + app().conf.str_instance_id);
                db_main.exec(q);

                trans.commit();

                clientCounter->needTotalSync = false;
            } else {
                db_main.exec(q);
            }

            clientCounter->fixChanges(marker);

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
