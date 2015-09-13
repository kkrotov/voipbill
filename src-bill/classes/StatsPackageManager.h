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
    long long int lastSaveCallTime;

    map<int, StatsPackage> statsPackage;
    map<int, list<int>> statsByPackageId;

    StatsPackageManager();
    bool ready() { return loaded; };
    void load(BDb * db);

    int getSeconds(int service_package_id, time_t connect_time);

    void prepareSaveQuery(stringstream &query);
    void executeSaveQuery(BDb * dbCalls, stringstream &query);

    void createNewPartition();
    void removePartitionAfterSave();

private:
    void add(CallInfo * callInfo);
    size_t size();
    friend class DataBillingContainer;
    friend class Billing;

    int getStatsPackageId(Call * call);
    void createStatsPackage(CallInfo *callInfo);
    void updateStatsPackage(Call *call, int statPackageId);
};