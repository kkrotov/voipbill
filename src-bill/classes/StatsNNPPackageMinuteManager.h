#pragma once

#include "../common.h"
#include "BDb.h"
#include "Spinlock.h"
#include "../models/StatsNNPPackageMinute.h"
#include "../models/CallInfo.h"

class StatsNNPPackageMinuteManager
{
private:
    int lastTrunkNNPPackageMinuteId = 0;

    vector<map<int, StatsNNPPackageMinute>> realtimeStatsNNPPackageMinuteParts;

    bool loaded = false;
public:
    map<int, StatsNNPPackageMinute> statsNNPPackageMinute;
    map<int, list<int>> statsByNNPPackageMinuteId;
    set<int> forSync;

    StatsNNPPackageMinuteManager();
    bool ready() { return loaded; };
    void load(BDb * db, time_t lastSaveCallTime);
    void recalc(BDb * db, long long int storedLastId);

    StatsNNPPackageMinute * getCurrent(time_t connect_time, Client * account, StatsNNPPackageMinute * nnpPackageMinute);


    void getChanges(map<int, StatsNNPPackageMinute> &changes);
    void addChanges(map<int, StatsNNPPackageMinute> &changes);


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

    StatsNNPPackageMinute * createStatsNNPPackageMinute(time_t connect_time, Client * account, StatsNNPPackageMinute * nnpPackageMinute);
    StatsNNPPackageMinute * updateStatsNNPPackageMinute(CallInfo *callInfo, int statNNPPackageMinuteId);

    int getUsedSeconds(int nnp_account_tariff_light_id, int nnp_package_minute_id);
};