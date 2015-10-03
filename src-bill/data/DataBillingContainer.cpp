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

    if (calls.isNeedCreatePartitionAfterAdd()) {
        createNewPartition();
    }
}

void DataBillingContainer::save(BDb * dbCalls) {

    {
        lock_guard<Spinlock> guard(lock);

        if (!calls.isNeedSave()) {
            return;
        }

        if (calls.isNeedCreatePartitionBeforeSave()) {
            createNewPartition();
        }
    }

    BDbTransaction trans(dbCalls);

    map<time_t, stringstream> callsQueryPerMonth;
    stringstream statAccountQuery;
    stringstream statFreeminQuery;
    stringstream statPackageQuery;

    {
        lock_guard<Spinlock> guard(lock);
        calls.prepareSaveQueries(callsQueryPerMonth);
    }
    {
        lock_guard<Spinlock> guard(lock);
        statsAccount.prepareSaveQuery(statAccountQuery);
    }
    {
        lock_guard<Spinlock> guard(lock);
        statsFreemin.prepareSaveQuery(statFreeminQuery);
    }
    {
        lock_guard<Spinlock> guard(lock);
        statsPackage.prepareSaveQuery(statPackageQuery);
    }

    calls.executeSaveQueries(dbCalls, callsQueryPerMonth);
    statsAccount.executeSaveQuery(dbCalls, statAccountQuery);
    statsFreemin.executeSaveQuery(dbCalls, statFreeminQuery);
    statsPackage.executeSaveQuery(dbCalls, statPackageQuery);

    trans.commit();

    {
        lock_guard<Spinlock> guard(lock);
        removePartitionAfterSave();
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
    calls.createNewPartition();
    statsAccount.createNewPartition();
    statsFreemin.createNewPartition();
    statsPackage.createNewPartition();
}

void DataBillingContainer::removePartitionAfterSave() {
    calls.removePartitionAfterSave();
    statsAccount.removePartitionAfterSave();
    statsFreemin.removePartitionAfterSave();
    statsPackage.removePartitionAfterSave();
}

bool DataBillingContainer::cdrsLoadPart(BDb * db_calls) {
    return cdrs.loadPart(db_calls);
}

size_t DataBillingContainer::cdrsQueueSize() {
    return cdrs.getQueueSize();
}

Cdr * DataBillingContainer::getFirstCdr() {
    return cdrs.getFirstCdr();
}

void DataBillingContainer::removeFirstCdr() {
    cdrs.removeFirstCdr();
}

long long int DataBillingContainer::getCdrsLastId() {
    return cdrs.getLastId();
}

time_t DataBillingContainer::getCdrsLastTime() {
    return cdrs.getLastTime();
}

size_t DataBillingContainer::getCdrsCounter() {
    return cdrs.getCounter();
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

void DataBillingContainer::statsAccountGetClients(vector<StatsAccount> &destClients) {
    lock_guard<Spinlock> guard(lock);
    statsAccount.getClients(destClients);
}

int DataBillingContainer::statsAccountGetSumMonth(int account_id, double vat_rate) {
    lock_guard<Spinlock> guard(lock);
    return statsAccount.getSumMonth(account_id, vat_rate);
}

int DataBillingContainer::statsAccountGetSumDay(int account_id, double vat_rate) {
    lock_guard<Spinlock> guard(lock);
    return statsAccount.getSumDay(account_id, vat_rate);
}

int DataBillingContainer::statsAccountGetSumBalance(int account_id, double vat_rate) {
    lock_guard<Spinlock> guard(lock);
    return statsAccount.getSumBalance(account_id, vat_rate);
}

int DataBillingContainer::statsFreeminGetSeconds(Call * call)
{
    lock_guard<Spinlock> guard(lock);
    return statsFreemin.getSeconds(call);
}

int DataBillingContainer::statsPackageGetSeconds(int service_package_id, time_t connect_time) {
    lock_guard<Spinlock> guard(lock);
    return statsPackage.getSeconds(service_package_id, connect_time);
}

void DataBillingContainer::statsAccountGetChanges(map<int, StatsAccount> &changes) {
    lock_guard<Spinlock> guard(lock);
    statsAccount.getChanges(changes);
}

void DataBillingContainer::statsAccountAddChanges(map<int, StatsAccount> &changes) {
    lock_guard<Spinlock> guard(lock);
    statsAccount.addChanges(changes);
}

void DataBillingContainer::statsFreeminGetChanges(map<int, StatsFreemin> &changes) {
    lock_guard<Spinlock> guard(lock);
    statsFreemin.getChanges(changes);
}

void DataBillingContainer::statsFreeminAddChanges(map<int, StatsFreemin> &changes) {
    lock_guard<Spinlock> guard(lock);
    statsFreemin.addChanges(changes);
}

void DataBillingContainer::statsPackageGetChanges(map<int, StatsPackage> &changes) {
    lock_guard<Spinlock> guard(lock);
    statsPackage.getChanges(changes);
}

void DataBillingContainer::statsPackageAddChanges(map<int, StatsPackage> &changes) {
    lock_guard<Spinlock> guard(lock);
    statsPackage.addChanges(changes);
}