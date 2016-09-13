#pragma once

#include "../common.h"
#include "../classes/BDb.h"
#include "../classes/Spinlock.h"
#include "../models/Cdr.h"
#include "../models/Call.h"
#include "../lists/ClientList.h"
#include "ClientLockData.h"
#include "../classes/AppBill.h"
#include "../classes/CdrManager.h"
#include "../classes/CallsManager.h"
#include "../classes/StatsAccountManager.h"
#include "../classes/StatsFreeminManager.h"
#include "../classes/StatsPackageManager.h"
#include "../classes/StatsTrunkSettingsManager.h"

#include "DataContainer.h"

class DataBillingContainer {
public:
    Spinlock lock;

    mutex fetchCdrLock;
    mutex calcCallsLock;
    mutex saveLock;
    mutex syncCallsCentralLock;
    mutex syncCountersCentralLock;
    mutex syncLocksCentralLock;

private:
    CdrManager          cdrs;
    CallsManager        calls;
public:
    StatsAccountManager         statsAccount;
    StatsFreeminManager         statsFreemin;
    StatsPackageManager         statsPackage;
    StatsTrunkSettingsManager   statsTrunkSettings;
    ClientLockData              clientLock;

    long long int lastSyncCentralCallId = -1;
    long long int lastSyncCentralCdrId = 0;
    long long int lastSyncCentralCdrUnfinishedId = 0;

    time_t lastSyncCentralCallTime = 0;
    time_t lastSyncCentralCdrTime = 0;

    static DataBillingContainer * instance();

    void loadAll(BDb * db, bool recalc = false);
    void getAccountIdsForRecalcAccountSum(DataContainer *data, list<int> &accountIds);
    void recalcAccountSum(BDb * db, list<int> &accountIds);
    bool ready();
    void addCall(CallInfo * callInfo);
    void save(BDb * dbCalls);

    void prepareSyncCallsCentral(BDb * db_main);


    bool cdrsLoadPart(BDb * db_calls);
    size_t cdrsQueueSize();
    Cdr * getFirstCdr();
    void removeFirstCdr();

    long long int getCdrsLastId();
    time_t getCdrsLastTime();
    size_t getCdrsCounter();

    size_t callsQueueSize();

    long long int getCallsLastId();
    time_t getCallsLastTime();
    size_t getCallsCounter();

    long long int getCallsStoredLastId();
    time_t getCallsStoredLastTime();
    size_t getCallsStoredCounter();

    void statsAccountGetClients(vector<StatsAccount> &destClients);
    double statsAccountGetSumMonth(int account_id, double vat_rate);
    double statsAccountGetSumDay(int account_id, double vat_rate);
    double statsAccountGetSumBalance(int account_id, double vat_rate);
    StatsFreemin * statsFreeminGetCurrent(CallInfo * callInfo);
    StatsPackage * statsPackageGetCurrent(CallInfo * callInfo, ServicePackage * servicePackage, TariffPackage * tariffPackage);
    StatsTrunkSettings * statsTrunkSettingsGetCurrent(time_t connect_time, Client * account, ServiceTrunkSettings * trunkSettings);
    void statsAccountGetChanges(map<int, StatsAccount> &changes, bool &needClear);
    void statsAccountAddChanges(map<int, StatsAccount> &changes, bool needClear);
    void statsFreeminGetChanges(map<int, StatsFreemin> &changes);
    void statsFreeminAddChanges(map<int, StatsFreemin> &changes);
    void statsPackageGetChanges(map<int, StatsPackage> &changes);
    void statsPackageAddChanges(map<int, StatsPackage> &changes);
    void statsTrunkSettingsGetChanges(map<int, StatsTrunkSettings> &changes);
    void statsTrunkSettingsAddChanges(map<int, StatsTrunkSettings> &changes);

private:
    void loadLastCallIdAndCdrIdAndTime(BDb * db_calls);
    void loadSyncCentralCallIdAndTime(BDb * db_main);
    void loadSyncCentralCdrIdAndTime(BDb * db_main);
    void loadSyncCentralCdrUnfinishedIdAndTime(BDb * db_main);
    void createNewPartition();
    void removePartitionAfterSave();

    friend class PageFmins;
    friend class PagePackage;
};
