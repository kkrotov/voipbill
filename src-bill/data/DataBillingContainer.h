#pragma once

#include "../common.h"
#include "../classes/BDb.h"
#include "../classes/Spinlock.h"
#include "../models/Cdr.h"
#include "../models/Call.h"
#include "ClientLockData.h"
#include "../classes/AppBill.h"
#include "../classes/CdrManager.h"
#include "../classes/CallsManager.h"
#include "../classes/StatsAccountManager.h"
#include "../classes/StatsFreeminManager.h"
#include "../classes/StatsPackageManager.h"

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
    StatsAccountManager statsAccount;
    StatsFreeminManager statsFreemin;
    StatsPackageManager statsPackage;
public:
    ClientLockData      clientLock;

    long long int lastSyncCentralCallId = -1;

    time_t lastSyncCentralCallTime = 0;

    static DataBillingContainer * instance();

    void loadAll(BDb * db);
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
    int statsAccountGetSumMonth(int account_id, double vat_rate);
    int statsAccountGetSumDay(int account_id, double vat_rate);
    int statsAccountGetSumBalance(int account_id, double vat_rate);
    int statsFreeminGetSeconds(Call * call);
    int statsPackageGetSeconds(int service_package_id, time_t connect_time);
    void statsAccountGetChanges(map<int, StatsAccount> &changes);
    void statsAccountAddChanges(map<int, StatsAccount> &changes);
    void statsFreeminGetChanges(map<int, StatsFreemin> &changes);
    void statsFreeminAddChanges(map<int, StatsFreemin> &changes);
    void statsPackageGetChanges(map<int, StatsPackage> &changes);
    void statsPackageAddChanges(map<int, StatsPackage> &changes);

private:
    void loadLastCallIdAndCdrIdAndTime(BDb * db_calls);
    void loadSyncCentralCallIdAndTime(BDb * db_main);
    void createNewPartition();
    void removePartitionAfterSave();

    friend class PageFmins;
    friend class PagePackage;
};