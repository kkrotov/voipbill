#include "ThreadSyncCounters.h"
#include "../classes/AppBill.h"

ThreadSyncCounters::ThreadSyncCounters() {
    id = idName();
    name = "Sync Counters to central db";

    db_main.setCS(app().conf.db_main);

    last_sync_account_count = 0;
    last_sync_freemin_count = 0;
    last_sync_package_count = 0;
    total_sync_account_count = 0;
    total_sync_freemin_count = 0;
    total_sync_package_count = 0;
}

bool ThreadSyncCounters::ready() {
    return repository.billingData->ready();
}

void ThreadSyncCounters::run() {

    unique_lock<mutex> lock(repository.billingData->syncCountersCentralLock, try_to_lock);
    if (!lock.owns_lock()) {
        return;
    }

    do_sync_account();

    do_sync_freemin();

    do_sync_package();
}

void ThreadSyncCounters::do_sync_account() {

    map<int, StatsAccount> changes;
    repository.billingData->statsAccountGetChanges(changes);

    stringstream query;
    query << "INSERT INTO billing.stats_account(server_id, account_id, amount_month, sum_month, amount_day, sum_day, amount_date, sum, min_call_id, max_call_id) VALUES\n";
    int i = 0;

    for (auto it: changes) {
        StatsAccount &stats = it.second;
        if (i > 0) query << ",\n";
        query << "(";
        query << "'" << app().conf.instance_id << "',";
        query << "'" << stats.account_id << "',";
        query << "'" << string_time(stats.amount_month) << "',";
        query << "'" << stats.sum_month << "',";
        query << "'" << string_time(stats.amount_day) << "',";
        query << "'" << stats.sum_day << "',";
        query << "'" << string_time(stats.amount_date) << "',";
        query << "'" << stats.sum << "',";
        query << "'" << stats.min_call_id << "',";
        query << "'" << stats.max_call_id << "')";
        i++;
    }

    if (changes.size() > 0) {
        try {
            db_main.exec(query.str());
        } catch (Exception &e) {
            e.addTrace("ThreadSyncCounters::do_sync_account:");
            repository.billingData->statsAccountAddChanges(changes);
            throw e;
        }
    }

    last_sync_account_count = changes.size();
    total_sync_account_count += changes.size();
}

void ThreadSyncCounters::do_sync_freemin() {

    map<int, StatsFreemin> changes;
    repository.billingData->statsFreeminGetChanges(changes);

    stringstream query;
    query << "INSERT INTO billing.stats_freemin(server_id, id, month_dt, service_number_id, used_seconds, used_credit, min_call_id, max_call_id) VALUES\n";

    int i = 0;
    for (auto it: changes) {
        StatsFreemin &stats = it.second;
        if (i > 0) query << ",\n";
        query << "(";
        query << "'" << app().conf.instance_id << "',";
        query << "'" << stats.id << "',";
        query << "'" << string_time(stats.month_dt) << "',";
        query << "'" << stats.service_number_id << "',";
        query << "'" << stats.used_seconds << "',";
        query << "'" << stats.used_credit << "',";
        query << "'" << stats.min_call_id << "',";
        query << "'" << stats.max_call_id << "')";
        i++;
    }

    if (changes.size() > 0) {
        try {
            db_main.exec(query.str());
        } catch (Exception &e) {
            e.addTrace("ThreadSyncCounters::do_sync_freemin:");
            repository.billingData->statsFreeminAddChanges(changes);
            throw e;
        }
    }

    last_sync_freemin_count = changes.size();
    total_sync_freemin_count += changes.size();
}

void ThreadSyncCounters::do_sync_package() {

    map<int, StatsPackage> changes;
    repository.billingData->statsPackageGetChanges(changes);

    stringstream query;
    query << "INSERT INTO billing.stats_package(server_id, id, package_id, used_seconds, used_credit, paid_seconds, activation_dt, expire_dt, min_call_id, max_call_id) VALUES\n";

    int i = 0;
    for (auto it: changes) {
        StatsPackage &stats = it.second;
        if (i > 0) query << ",\n";
        query << "(";
        query << "'" << app().conf.instance_id << "',";
        query << "'" << stats.id << "',";
        query << "'" << stats.package_id << "',";
        query << "'" << stats.used_seconds << "',";
        query << "'" << stats.used_credit << "',";
        query << "'" << stats.paid_seconds << "',";
        query << "'" << string_time(stats.activation_dt) << "',";
        query << "'" << string_time(stats.expire_dt) << "',";
        query << "'" << stats.min_call_id << "',";
        query << "'" << stats.max_call_id << "')";
        i++;
    }

    if (changes.size() > 0) {
        try {
            db_main.exec(query.str());
        } catch (Exception &e) {
            e.addTrace("ThreadSyncCounters::do_sync_package:");
            repository.billingData->statsPackageAddChanges(changes);
            throw e;
        }
    }

    last_sync_package_count = changes.size();
    total_sync_package_count += changes.size();
}

void ThreadSyncCounters::htmlfull(stringstream &html) {
    this->html(html);

    html << "Last sync account count: " << last_sync_account_count << "<br/>\n";
    html << "Last sync freemin count: " << last_sync_freemin_count << "<br/>\n";
    html << "Last sync package count: " << last_sync_package_count << "<br/>\n";
    html << "Total sync account count: " << total_sync_account_count << "<br/>\n";
    html << "Total sync freemin count: " << total_sync_freemin_count << "<br/>\n";
    html << "Total sync package count: " << total_sync_package_count << "<br/>\n";
}

bool ThreadSyncCounters::hasFullHtml() {
    return true;
}
