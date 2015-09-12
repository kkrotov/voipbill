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

    map<int, StatsPackage> storedStatsPackage;
    map<int, list<int>> storedStatsByPackageId;

    vector<map<int, StatsPackage>> realtimeStatsPackageParts;
    vector<map<int, list<int>>> realtimeStatsByPackageIdParts;

    bool loaded = false;
public:
    long long int lastSaveCallTime;

    Spinlock lock;

    StatsPackageManager();
    bool ready() { return loaded; };
    void load(BDb * db);

    int getSeconds(Call * call);
    int getSeconds(int service_package_id, time_t connect_time);

    void save(BDb * dbCalls);

    void createNewPartition();
    void afterSave();

private:
    void add(CallInfo * callInfo);
    size_t size();
    friend class DataBillingContainer;
    friend class Billing;

    int getSeconds(int service_package_id, time_t connect_time, map<int, list<int>> &freeminsByServiceId, map<int, StatsPackage> &statsFreemin);
    void createStatsPackage(CallInfo *callInfo);
    bool updateStatsPackage(CallInfo *callInfo);
    void move(
            map<int, StatsPackage> &fromStatsPackage, map<int, list<int>> &fromStatsByPackageId,
            map<int, StatsPackage> &toStatsPackage, map<int, list<int>> &toStatsByPackageId
    );

};