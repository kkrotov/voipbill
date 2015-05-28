#include "ThreadSyncLocks.h"
#include "../classes/AppBill.h"

ThreadSyncLocks::ThreadSyncLocks() {
    id = idName();
    name = "Sync Locks to central db";

    db_main.setCS(app().conf.db_main);

    billingData = DataBillingContainer::instance();

    last_sync_count = 0;
    total_sync_count = 0;
}

bool ThreadSyncLocks::ready() {
    return billingData->ready();
}

void ThreadSyncLocks::run() {

    unique_lock<mutex> lock(billingData->syncLocksCentralLock, try_to_lock);
    if (!lock.owns_lock()) {
        return;
    }

    save_client_locks();

}

void ThreadSyncLocks::save_client_locks() {

    string q;

    auto clientLock = billingData->clientLock.get();

    vector<int> changes;
    unsigned long long int marker;
    bool needTotalSync;

    clientLock->getChanges(marker, changes, needTotalSync);

    int sync_count = 0;

    for (int account_id : changes) {
        ClientLockObj value = clientLock->get(account_id);

        if (sync_count == 0) {
            q.append("INSERT INTO billing.clients_locks(client_id,region_id,voip_auto_disabled,voip_auto_disabled_local)VALUES");
        } else {
            q.append(",");
        }
        q.append(string_fmt("(%d,'%d',%s,%s)",
                            account_id,
                            app().conf.instance_id,
                            (value.disabled_global ? "true" : "false"),
                            (value.disabled_local ? "true" : "false")));
        sync_count += 1;
    }

    if (sync_count > 0) {
        try {
            if (needTotalSync) {
                BDbTransaction trans(&db_main);

                db_main.exec("DELETE FROM billing.clients_locks WHERE region_id=" + app().conf.str_instance_id);
                db_main.exec(q);

                trans.commit();

                clientLock->needTotalSync = false;
            } else {
                db_main.exec(q);
            }

            clientLock->fixChanges(marker);

        } catch (Exception &e) {
            e.addTrace("ThreadSyncLocks::save_client_locks:");
            throw e;
        }
    }

    last_sync_count = sync_count;
    total_sync_count += sync_count;
}

void ThreadSyncLocks::htmlfull(stringstream &html) {
    this->html(html);

    html << "Last sync count: " << last_sync_count << "<br/>\n";
    html << "Last sync count: " << total_sync_count << "<br/>\n";
}

bool ThreadSyncLocks::hasFullHtml() {
    return true;
}
