#include "DataBillingContainer.h"

DataBillingContainer * DataBillingContainer::instance() {
    static DataBillingContainer inst;
    return &inst;
}

void DataBillingContainer::loadAll(BDb * db) {

    loadLastCallIdAndCdrIdAndTime(db);

    statsAccount.load(db);
    statsFreemin.load(db);
    statsPackage.load(db);

    clientLock.load(db);
}

bool DataBillingContainer::ready() {

    if (cdrs.getLastId() < 0 || calls.getLastId() < 0 || calls.getStoredLastId() < 0) {
        return false;
    }

    if (!statsAccount.ready()) {
        return false;
    }

    if (!statsFreemin.ready()) {
        return false;
    }

    if (!statsPackage.ready()) {
        return false;
    }

    if (!clientLock.ready()) {
        return false;
    }


    return true;
}

void DataBillingContainer::save(BDb * dbCalls) {

    try {
        calls.moveRealtimeToTemp();
        statsAccount.moveRealtimeToTemp();
        statsFreemin.moveRealtimeToTemp();
        statsPackage.moveRealtimeToTemp();

        BDbTransaction trans(dbCalls);

        calls.save(dbCalls);
        statsAccount.save(dbCalls);
        statsFreemin.save(dbCalls);
        statsPackage.save(dbCalls);

        trans.commit();

        calls.moveTempToStored();
        statsAccount.moveTempToStored();
        statsFreemin.moveTempToStored();
        statsPackage.moveTempToStored();
    } catch (Exception &e) {
        throw e;
    }

}

void DataBillingContainer::prepareSyncCallsCentral(BDb * db_main) {
    try {
        loadSyncCentralCallIdAndTime(db_main);
    } catch (Exception &e) {
        e.addTrace("DataBillingContainer::prepareSyncCallsCentral");
        throw e;
    }
}

void DataBillingContainer::loadLastCallIdAndCdrIdAndTime(BDb * db_calls) {
    auto res = db_calls->query("SELECT MAX(cdr_id), MAX(id), MAX(connect_time) FROM calls_raw.calls_raw");
    if (res.next()) {
        cdrs.setLastId(res.get_ll(0));
        calls.setStoredLastId(res.get_ll(1));
        calls.setStoredLastTime(parseDateTime(res.get(2)));
    } else {
        cdrs.setLastId(0);
        calls.setStoredLastId(0);
        calls.setStoredLastTime(0);
    }

    cdrs.setLastTime(calls.getStoredLastTime());
    calls.setLastId(calls.getStoredLastId());
    calls.setLastTime(calls.getStoredLastTime());
}

void DataBillingContainer::loadSyncCentralCallIdAndTime(BDb * db_main) {
    auto res = db_main->query("select id, connect_time from calls_raw.calls_raw where server_id = " + app().conf.str_instance_id + " order by id desc limit 1");
    if (res.next()) {
        lastSyncCentralCallId   = res.get_ll(0);
        lastSyncCentralCallTime = parseDateTime(res.get(1));
    } else {
        lastSyncCentralCallId   = 0;
        lastSyncCentralCallTime = 0;
    }
}