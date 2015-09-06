#include "ThreadSyncCalls.h"
#include "../classes/AppBill.h"

ThreadSyncCalls::ThreadSyncCalls() {
    id = idName();
    name = "Sync Calls to central db";

    db_main.setCS(app().conf.db_main);
    db_calls.setCS(app().conf.db_calls);
}

void ThreadSyncCalls::run() {

    unique_lock<mutex> lock(repository.billingData->syncCallsCentralLock, try_to_lock);
    if (!lock.owns_lock()) {
        return;
    }

    return;

    repository.billingData->prepareSyncCallsCentral(&db_main);

    string local_sync_month;
    string local_prev_sync_month;
    try {
        BDbResult res = db_calls.query("select date_trunc('month', connect_time), date_trunc('month', connect_time) - interval '1 month' from calls_raw.calls_raw where id>" + lexical_cast<string>(repository.billingData->lastSyncCentralCallId) + " order by id limit 1");
        if (!res.next()) {
            // nothing to sync
            return;
        }
        local_sync_month += res.get_s(0).substr(0, 4);
        local_sync_month += res.get_s(0).substr(5, 2);
        local_prev_sync_month += res.get_s(1).substr(0, 4);
        local_prev_sync_month += res.get_s(1).substr(5, 2);
    } catch (Exception &e) {
        e.addTrace("ThreadSyncCalls::run::get_local_sync_month");
        throw e;
    }

    try {
        BDbTransaction trans(&db_calls);

        copyCallsPart(local_prev_sync_month);

        copyCallsPart(local_sync_month);

        trans.commit();

    } catch (Exception &e) {
        e.addTrace("ThreadSyncCalls::run::copy(main_last_id:" + lexical_cast<string>(repository.billingData->lastSyncCentralCallId) + ")");
        throw e;
    }
}

void ThreadSyncCalls::copyCallsPart(string month) {
    BDb::copy("calls_raw.calls_raw_" + month,
        "",
        "       id, orig, peer_id, cdr_id, connect_time, trunk_id, account_id, trunk_service_id, number_service_id, src_number, dst_number, billed_time, rate, cost, tax_cost, interconnect_rate, interconnect_cost, service_package_id, service_package_limit_id, package_time, package_credit, destination_id, pricelist_id, prefix, geo_id, geo_operator_id, mob, geo_mob, operator_id, server_id",
        "select id, orig, peer_id, cdr_id, connect_time, trunk_id, account_id, trunk_service_id, number_service_id, src_number, dst_number, billed_time, rate, cost, tax_cost, interconnect_rate, interconnect_cost, service_package_id, service_package_limit_id, package_time, package_credit, destination_id, pricelist_id, prefix, geo_id, geo_operator_id, mob, geo_mob, operator_id, " + app().conf.str_instance_id + " " \
        "   from calls_raw.calls_raw_" + month +
        "   where id>" + lexical_cast<string>(repository.billingData->lastSyncCentralCallId) +
        "   order by id limit 100000",
        &db_calls, &db_main);
}

