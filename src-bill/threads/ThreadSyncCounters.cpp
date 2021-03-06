#include "ThreadSyncCounters.h"
#include "../classes/AppBill.h"

ThreadSyncCounters::ThreadSyncCounters() {
    id = idName();
    name = "Sync Counters to central db";

    db_main.setCS(app().conf.db_main);
    db_calls.setCS(app().conf.db_calls);

    last_sync_account_count = 0;
    last_sync_freemin_count = 0;
    last_sync_package_count = 0;
    last_sync_trunk_settings_count = 0;
    last_sync_nnp_package_minute_count = 0;

    total_sync_account_count = 0;
    total_sync_freemin_count = 0;
    total_sync_package_count = 0;
    total_sync_trunk_settings_count = 0;
    total_sync_nnp_package_minute_count = 0;
}

bool ThreadSyncCounters::ready() {
    return repository.billingData->ready();
}

void ThreadSyncCounters::run() {

    unique_lock<mutex> lock(repository.billingData->syncCountersCentralLock, try_to_lock);
    if (!lock.owns_lock()) {
        return;
    }

    last_sync_account_count = repository.billingData->statsAccount.sync(&db_main, repository.billingData);
    total_sync_account_count += last_sync_account_count;

    last_sync_freemin_count = repository.billingData->statsFreemin.sync(&db_main, &db_calls);
    total_sync_freemin_count += last_sync_freemin_count;

    last_sync_package_count = repository.billingData->statsPackage.sync(&db_main, &db_calls);
    total_sync_package_count += last_sync_package_count;

    last_sync_trunk_settings_count = repository.billingData->statsTrunkSettings.sync(&db_main, &db_calls);
    total_sync_trunk_settings_count += last_sync_trunk_settings_count;

    last_sync_nnp_package_minute_count = repository.billingData->statsNNPPackageMinute.sync(&db_main, &db_calls);
    total_sync_nnp_package_minute_count += last_sync_nnp_package_minute_count;
}

void ThreadSyncCounters::htmlfull(stringstream &html) {
    this->html(html);

    html << "Last sync account count: " << last_sync_account_count << "<br/>\n";
    html << "Last sync freemin count: " << last_sync_freemin_count << "<br/>\n";
    html << "Last sync package count: " << last_sync_package_count << "<br/>\n";
    html << "Last sync tunk stettings count: " << last_sync_trunk_settings_count << "<br/>\n";
    html << "Last sync nnp package minute count: " << last_sync_nnp_package_minute_count << "<br/>\n";

    html << "Total sync account count: " << total_sync_account_count << "<br/>\n";
    html << "Total sync freemin count: " << total_sync_freemin_count << "<br/>\n";
    html << "Total sync package count: " << total_sync_package_count << "<br/>\n";
    html << "Total sync tunk stettings count: " << total_sync_trunk_settings_count << "<br/>\n";
    html << "Total sync nnp package minute count: " << total_sync_nnp_package_minute_count << "<br/>\n";

}
bool ThreadSyncCounters::hasFullHtml() {
    return true;
}
