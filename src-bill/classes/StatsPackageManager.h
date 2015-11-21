#pragma once

#include "../common.h"
#include "BDb.h"
#include "Spinlock.h"
#include "../models/StatsPackage.h"
#include "../models/CallInfo.h"

class StatsPackageManager
{
private:
    int lastPackageStatId = 0;

    vector<map<int, StatsPackage>> realtimeStatsPackageParts;

    bool loaded = false;
public:
    map<int, StatsPackage> statsPackage;
    map<int, list<int>> statsByPackageId;
    set<int> forSync;

    StatsPackageManager();
    bool ready() { return loaded; };
    void load(BDb * db, time_t lastSaveCallTime);
    void recalc(BDb * db, long long int storedLastId);
    StatsPackage * getCurrent(CallInfo * callInfo, ServicePackage * servicePackage, TariffPackage * tariffPackage);
    void getChanges(map<int, StatsPackage> &changes);
    void addChanges(map<int, StatsPackage> &changes);


    void prepareSaveQuery(stringstream &query);
    void executeSaveQuery(BDb * dbCalls, stringstream &query);

    void createNewPartition();
    void removePartitionAfterSave();

    size_t sync(BDb * db_main, BDb * db_calls);

private:
    void add(CallInfo * callInfo);
    size_t size();
    friend class DataBillingContainer;
    friend class Billing;

    StatsPackage * createStatsPackage(CallInfo * callInfo, ServicePackage * servicePackage, TariffPackage * tariffPackage);
    StatsPackage * updateStatsPackage(CallInfo *callInfo, int statPackageId);
};