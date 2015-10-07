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
    bool needClear;
    repository.billingData->statsAccountGetChanges(changes, needClear);

    stringstream query;
    query << "INSERT INTO billing.stats_account(server_id, account_id, amount_month, sum_month, amount_day, sum_day, amount_date, sum) VALUES\n";
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
        query << "'" << stats.sum << "')";
        i++;
    }

    if (changes.size() > 0) {
        try {
            if (needClear) {
                db_main.exec("BEGIN");
                db_main.exec("DELETE FROM billing.stats_account WHERE server_id = " + app().conf.str_instance_id);
            }
            db_main.exec(query.str());
            if (needClear) {
                db_main.exec("COMMIT");
            }
        } catch (Exception &e) {
            repository.billingData->statsAccountAddChanges(changes);
            if (needClear) {
                db_main.exec("ROLLBACK");
            }
            e.addTrace("ThreadSyncCounters::do_sync_account:");
            throw e;
        }
    }

    last_sync_account_count = changes.size();
    total_sync_account_count += changes.size();
}

void ThreadSyncCounters::do_sync_freemin() {

    BDbResult resMax = db_main.query("SELECT max(max_call_id) FROM billing.stats_freemin WHERE server_id='" + app().conf.str_instance_id +"'");
    long long int central_max_call_id = resMax.next() ? resMax.get_ll(0) : 0;

    BDbResult res = db_calls.query(
            " SELECT id, month_dt, service_number_id, used_seconds, used_credit, min_call_id, max_call_id " \
            " from billing.stats_freemin " \
            " where max_call_id > '" + lexical_cast<string>(central_max_call_id) + "'");

    stringstream query;
    query << "INSERT INTO billing.stats_freemin(server_id, id, month_dt, service_number_id, used_seconds, used_credit, min_call_id, max_call_id) VALUES\n";

    int i = 0;
    while (res.next()) {
        if (i > 0) query << ",\n";
        query << "(";
        query << "'" << app().conf.instance_id << "',";
        query << "'" << res.get(0) << "',";
        query << "'" << res.get(1) << "',";
        query << "'" << res.get(2) << "',";
        query << "'" << res.get(3) << "',";
        query << "'" << res.get(4) << "',";
        query << "'" << res.get(5) << "',";
        query << "'" << res.get(6) << "')";
        i++;
    }

    if (res.size() > 0) {
        try {
            db_main.exec(query.str());
        } catch (Exception &e) {
            e.addTrace("ThreadSyncCounters::do_sync_freemin:");
            throw e;
        }
    }

    last_sync_freemin_count = res.size();
    total_sync_freemin_count += res.size();
}

void ThreadSyncCounters::do_sync_package() {

    BDbResult resMax = db_main.query("SELECT max(max_call_id) FROM billing.stats_package WHERE server_id='" + app().conf.str_instance_id +"'");
    long long int central_max_call_id = resMax.next() ? resMax.get_ll(0) : 0;

    BDbResult res = db_calls.query(
            " SELECT id, package_id, used_seconds, used_credit, paid_seconds, activation_dt, expire_dt, min_call_id, max_call_id " \
            " from billing.stats_package " \
            " where max_call_id > '" + lexical_cast<string>(central_max_call_id) + "'");

    stringstream query;
    query << "INSERT INTO billing.stats_package(server_id, id, package_id, used_seconds, used_credit, paid_seconds, activation_dt, expire_dt, min_call_id, max_call_id) VALUES\n";

    int i = 0;
    while (res.next()) {
        if (i > 0) query << ",\n";
        query << "(";
        query << "'" << app().conf.instance_id << "',";
        query << "'" << res.get(0) << "',";
        query << "'" << res.get(1) << "',";
        query << "'" << res.get(2) << "',";
        query << "'" << res.get(3) << "',";
        query << "'" << res.get(4) << "',";
        query << "'" << res.get(5) << "',";
        query << "'" << res.get(6) << "')";
        query << "'" << res.get(7) << "',";
        query << "'" << res.get(8) << "',";
        i++;
    }

    if (res.size() > 0) {
        try {
            db_main.exec(query.str());
        } catch (Exception &e) {
            e.addTrace("ThreadSyncCounters::do_sync_package:");
            throw e;
        }
    }

    last_sync_package_count = res.size();
    total_sync_package_count += res.size();
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
