#pragma once

#include "../common.h"
#include "Spinlock.h"
#include "BDb.h"
#include "../models/StatsFreemin.h"
#include "../models/CallInfo.h"

class StatsFreeminManager
{
private:
    int lastStatsFreeminId = 0;
    bool loaded = false;
public:
    vector<map<int, StatsFreemin>> realtimeStatsFreeminParts;
    map<int, StatsFreemin> statsFreemin;
    map<int, list<int>> freeminsByServiceId;
    set<int> forSync;

    StatsFreeminManager();
    bool ready() { return loaded; };
    void load(BDb * db, time_t lastStoredCallTime);
    void recalc(BDb * db, long long int storedLastId);
    StatsFreemin * getCurrent(CallInfo * callInfo);
    void getChanges(map<int, StatsFreemin> &changes);
    void addChanges(map<int, StatsFreemin> &changes);

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

    StatsFreemin * createStatsFreemin(CallInfo * callInfo);
    StatsFreemin * updateStatsFreemin(CallInfo * callInfo, int statFreeminId);
};
