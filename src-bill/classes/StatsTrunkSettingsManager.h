#pragma once

#include "../common.h"
#include "BDb.h"
#include "Spinlock.h"
#include "../models/StatsTrunkSettings.h"
#include "../models/CallInfo.h"

class StatsTrunkSettingsManager
{
private:
    int lastTrunkSettingsStatId = 0;

    vector<map<int, StatsTrunkSettings>> realtimeStatsTrunkSettingsParts;

    bool loaded = false;
public:
    map<int, StatsTrunkSettings> statsTrunkSettings;
    map<int, list<int>> statsByTrunkSettingsId;
    set<int> forSync;

    StatsTrunkSettingsManager();
    bool ready() { return loaded; };
    void load(BDb * db, time_t lastSaveCallTime);
    void recalc(BDb * db, long long int storedLastId);
    StatsTrunkSettings * getCurrent(time_t connect_time, Client * account, ServiceTrunkSettings * trunkSettings);
    void getChanges(map<int, StatsTrunkSettings> &changes);
    void addChanges(map<int, StatsTrunkSettings> &changes);


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

    StatsTrunkSettings * createStatsTrunkSettings(time_t connect_time, Client * account, ServiceTrunkSettings * trunkSettings);
    StatsTrunkSettings * updateStatsTrunkSettings(CallInfo *callInfo, int statTrunkSettingsId);
};