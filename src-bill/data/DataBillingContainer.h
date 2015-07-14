#pragma once

#include "../common.h"
#include "../classes/BDb.h"
#include "../classes/Spinlock.h"
#include "../models/Cdr.h"
#include "../models/Call.h"
#include "StatsPackageData.h"
#include "ClientCounterData.h"
#include "ClientLockData.h"
#include "FminCounterData.h"
#include "../classes/AppBill.h"
#include "../classes/CdrManager.h"
#include "../classes/CallsManager.h"
#include "../classes/StatsAccountManager.h"
#include "../classes/StatsFreeminManager.h"
#include "../classes/StatsPackageManager.h"

class DataBillingContainer {
public:
    mutex fetchCdrLock;
    mutex calcCallsLock;
    mutex saveLock;
    mutex syncCallsCentralLock;
    mutex syncCountersCentralLock;
    mutex syncLocksCentralLock;

    CdrManager          cdrs;
    CallsManager        calls;
    StatsAccountManager statsAccount;
    StatsFreeminManager statsFreemin;
    StatsPackageManager statsPackage;

    StatsPackageData    statsPackageCounter;
    ClientCounterData   clientCounter;
    FminCounterData     fminCounter;
    ClientLockData      clientLock;

    long long int lastSyncCentralCallId = -1;

    time_t lastSyncCentralCallTime = 0;

    static DataBillingContainer * instance();

    void loadAll(BDb * db);
    bool ready();

    void prepareSyncCallsCentral(BDb * db_main);

private:
    void loadLastCallIdAndCdrIdAndTime(BDb * db_calls);
    void loadSyncCentralCallIdAndTime(BDb * db_main);
};