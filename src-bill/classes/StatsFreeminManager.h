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
    long long int lastStoredCallTime;

    vector<map<int, StatsFreemin>> realtimeStatsFreeminParts;
    map<int, StatsFreemin> statsFreemin;
    map<int, list<int>> freeminsByServiceId;
    set<int> forSync;

    StatsFreeminManager();
    bool ready() { return loaded; };
    void load(BDb * db);
    int getSeconds(Call * call);
    void getChanges(map<int, StatsFreemin> &changes);
    void addChanges(map<int, StatsFreemin> &changes);

    void prepareSaveQuery(stringstream &query);
    void executeSaveQuery(BDb * dbCalls, stringstream &query);

    void createNewPartition();
    void removePartitionAfterSave();

private:
    void add(CallInfo * callInfo);
    size_t size();
    friend class DataBillingContainer;
    friend class Billing;

    int getStatsFreeminId(Call * call);
    StatsFreemin * createStatsFreemin(Call * call);
    StatsFreemin * updateStatsFreemin(Call * call, int statFreeminId);
};
