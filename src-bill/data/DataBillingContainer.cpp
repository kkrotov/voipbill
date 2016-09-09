#include "DataBillingContainer.h"

DataBillingContainer * DataBillingContainer::instance() {
    static DataBillingContainer inst;
    return &inst;
}

void DataBillingContainer::loadAll(BDb * db, bool recalc) {

    cdrs.clear();
    calls.clear();

    loadLastCallIdAndCdrIdAndTime(db);

    if (recalc) {
        statsAccount.recalc(db);
        statsFreemin.recalc(db, getCallsStoredLastId());
        statsPackage.recalc(db, getCallsStoredLastId());
        statsTrunkSettings.recalc(db, getCallsStoredLastId());
        statsNNPPackageMinute.recalc(db, getCallsStoredLastId());
    }

    statsAccount.load(db);
    statsFreemin.load(db, getCallsStoredLastTime());
    statsPackage.load(db, getCallsStoredLastTime());
    statsTrunkSettings.load(db, getCallsStoredLastTime());
    statsNNPPackageMinute.load(db, getCallsStoredLastTime());

    clientLock.load(db);
}

void DataBillingContainer::getAccountIdsForRecalcAccountSum(DataContainer *data, list<int> &accountIds) {
    lock_guard<Spinlock> guard(lock);

    shared_ptr<ActiveCounter> activeCounter = data->activeCounter.get();
    shared_ptr<ClientList> clients = data->client.get();

    for (int accountId : activeCounter->activeClients) {
        Client * client = clients->find(accountId);
        if (client == nullptr) {
            continue;
        }

        StatsAccount * account = statsAccount.get(client->id);
        if (account == nullptr || account->amount_date != client->amount_date) {
            accountIds.push_back(client->id);
        }
    }
}

void DataBillingContainer::recalcAccountSum(BDb * db, list<int> &accountIds) {

    statsAccount.reloadSum(db, accountIds, lock);

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

    if (!statsTrunkSettings.ready()) {
        return false;
    }

    if (!statsNNPPackageMinute.ready()) {
        return false;
    }

    if (!clientLock.ready()) {
        return false;
    }

    return true;
}

void DataBillingContainer::addCall(CallInfo * callInfo) {
    lock_guard<Spinlock> guard(lock);

    statsAccount.add(callInfo);
    statsFreemin.add(callInfo);
    statsPackage.add(callInfo);
    statsTrunkSettings.add(callInfo);
    statsNNPPackageMinute.add(callInfo);

    calls.add(*callInfo->call);

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
    stringstream statTrunkSettingsQuery;
    stringstream statNNPPackageMinuteQuery;

    {
        lock_guard<Spinlock> guard(lock);
        vector<string> queryTime;
        calls.prepareSaveQueries(callsQueryPerMonth, queryTime);
        for (string qtime : queryTime)
            calls.createIfNotExists(dbCalls, qtime);
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
    {
        lock_guard<Spinlock> guard(lock);
        statsTrunkSettings.prepareSaveQuery(statTrunkSettingsQuery);
    }
    {
        lock_guard<Spinlock> guard(lock);
        statsNNPPackageMinute.prepareSaveQuery(statNNPPackageMinuteQuery);
    }

    calls.executeSaveQueries(dbCalls, callsQueryPerMonth);
    statsAccount.executeSaveQuery(dbCalls, statAccountQuery);
    statsFreemin.executeSaveQuery(dbCalls, statFreeminQuery);
    statsPackage.executeSaveQuery(dbCalls, statPackageQuery);
    statsTrunkSettings.executeSaveQuery(dbCalls, statTrunkSettingsQuery);
    statsNNPPackageMinute.executeSaveQuery(dbCalls, statNNPPackageMinuteQuery);

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
    statsTrunkSettings.createNewPartition();
    statsNNPPackageMinute.createNewPartition();
}

void DataBillingContainer::removePartitionAfterSave() {
    calls.removePartitionAfterSave();
    statsAccount.removePartitionAfterSave();
    statsFreemin.removePartitionAfterSave();
    statsPackage.removePartitionAfterSave();
    statsTrunkSettings.removePartitionAfterSave();
    statsNNPPackageMinute.removePartitionAfterSave();

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

double DataBillingContainer::statsAccountGetSumMonth(int account_id, double vat_rate) {
    lock_guard<Spinlock> guard(lock);
    return statsAccount.getSumMonth(account_id, vat_rate);
}

double DataBillingContainer::statsAccountGetSumDay(int account_id, double vat_rate) {
    lock_guard<Spinlock> guard(lock);
    return statsAccount.getSumDay(account_id, vat_rate);
}

double DataBillingContainer::statsAccountGetSumBalance(int account_id, double vat_rate) {
    lock_guard<Spinlock> guard(lock);
    return statsAccount.getSumBalance(account_id, vat_rate);
}

StatsFreemin * DataBillingContainer::statsFreeminGetCurrent(CallInfo * callInfo)
{
    lock_guard<Spinlock> guard(lock);
    return statsFreemin.getCurrent(callInfo);
}

StatsPackage * DataBillingContainer::statsPackageGetCurrent(CallInfo * callInfo, ServicePackage * servicePackage, TariffPackage * tariffPackage) {
    lock_guard<Spinlock> guard(lock);
    return statsPackage.getCurrent(callInfo, servicePackage, tariffPackage);
}

StatsTrunkSettings * DataBillingContainer::statsTrunkSettingsGetCurrent(time_t connect_time, Client * account, ServiceTrunkSettings * trunkSettings) {
    lock_guard<Spinlock> guard(lock);
    return statsTrunkSettings.getCurrent(connect_time, account, trunkSettings);
}

void DataBillingContainer::statsAccountGetChanges(map<int, StatsAccount> &changes, bool &needClear) {
    lock_guard<Spinlock> guard(lock);
    statsAccount.getChanges(changes, needClear);
}

void DataBillingContainer::statsAccountAddChanges(map<int, StatsAccount> &changes, bool needClear) {
    lock_guard<Spinlock> guard(lock);
    statsAccount.addChanges(changes, needClear);
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

void DataBillingContainer::statsTrunkSettingsGetChanges(map<int, StatsTrunkSettings> &changes) {
    lock_guard<Spinlock> guard(lock);
    statsTrunkSettings.getChanges(changes);
}

void DataBillingContainer::statsTrunkSettingsAddChanges(map<int, StatsTrunkSettings> &changes) {
    lock_guard<Spinlock> guard(lock);
    statsTrunkSettings.addChanges(changes);
}


void DataBillingContainer::statsNNPPackaeMinuteGetChanges(map<int, StatsNNPPackageMinute> &changes){
    lock_guard<Spinlock> guard(lock);
    statsNNPPackageMinute.getChanges(changes);
}

void DataBillingContainer::statsNNPPackaeMinuteAddChanges(map<int, StatsNNPPackageMinute> &changes){
    lock_guard<Spinlock> guard(lock);
    statsNNPPackageMinute.addChanges(changes);
}

