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
    lock_guard<Spinlock> guard(lock);

    if (!cdrs.ready()) {
        return false;
    }

    if (!calls.ready()) {
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

void DataBillingContainer::addCall(CallInfo * callInfo) {
    lock_guard<Spinlock> guard(lock);

    calls.add(*callInfo->call);
    statsAccount.add(callInfo);
    statsFreemin.add(callInfo);
    statsPackage.add(callInfo);

    createNewPartition();
}

void DataBillingContainer::save(BDb * dbCalls) {

    try {
        BDbTransaction trans(dbCalls);

        calls.save(dbCalls);
        statsAccount.save(dbCalls);
        statsFreemin.save(dbCalls);
        statsPackage.save(dbCalls);

        trans.commit();

        {
            lock_guard<Spinlock> guard(lock);
            afterSave();
        }

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

void DataBillingContainer::createNewPartition() {
    if (calls.getLastRealtimePartSize() >= CALLS_PARTITION_SIZE) {
        calls.createNewPartition();
        statsAccount.createNewPartition();
        statsFreemin.createNewPartition();
        statsPackage.createNewPartition();
    }
}

void DataBillingContainer::afterSave() {
    calls.afterSave();
    statsAccount.afterSave();
    statsFreemin.afterSave();
    statsPackage.afterSave();
}

size_t DataBillingContainer::cdrsQueueSize() {
    lock_guard<Spinlock> guard(lock);
    return cdrs.queue.size();
}

long long int DataBillingContainer::getCdrsLastId() {
    lock_guard<Spinlock> guard(lock);
    return cdrs.lastId;
}

time_t DataBillingContainer::getCdrsLastTime() {
    lock_guard<Spinlock> guard(lock);
    return cdrs.lastTime;
}

size_t DataBillingContainer::getCdrsCounter() {
    lock_guard<Spinlock> guard(lock);
    return cdrs.counter;
}

size_t DataBillingContainer::callsQueueSize() {
    lock_guard<Spinlock> guard(lock);
    return calls.getQueueSize();
}

long long int DataBillingContainer::getCallsLastId() {
    lock_guard<Spinlock> guard(lock);
    return calls.getLastId();
}

time_t DataBillingContainer::getCallsLastTime() {
    lock_guard<Spinlock> guard(lock);
    return calls.getLastTime();
}

size_t DataBillingContainer::getCallsCounter() {
    lock_guard<Spinlock> guard(lock);
    return calls.getCounter();
}

long long int DataBillingContainer::getCallsStoredLastId() {
    lock_guard<Spinlock> guard(lock);
    return calls.getStoredLastId();
}

time_t DataBillingContainer::getCallsStoredLastTime() {
    lock_guard<Spinlock> guard(lock);
    return calls.getStoredLastTime();
}

size_t DataBillingContainer::getCallsStoredCounter() {
    lock_guard<Spinlock> guard(lock);
    return calls.getStoredCounter();
}