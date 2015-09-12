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

    Spinlock lock;

    map<int, StatsFreemin> storedStatsFreemin;
    map<int, list<int>> storedFreeminsByServiceId;

    vector<map<int, StatsFreemin>> realtimeStatsFreeminParts;
    vector<map<int, list<int>>> realtimeFreeminsByServiceIdParts;

    StatsFreeminManager();
    bool ready() { return loaded; };
    void load(BDb * db);
    int getSeconds(Call * call);

    void save(BDb * dbCalls);

    void createNewPartition();
    void afterSave();

private:
    void add(CallInfo * callInfo);
    size_t size();
    friend class DataBillingContainer;
    friend class Billing;

    int getSeconds(Call * call, map<int, list<int>> &freeminsByServiceId, map<int, StatsFreemin> &statsFreemin);
    void createStatsFreemin(Call * call);
    bool updateStatsFreemin(Call * call);
    void move(
            map<int, StatsFreemin> &fromStatsFreemin, map<int, list<int>> &fromFreeminsByServiceId,
            map<int, StatsFreemin> &toStatsFreemin, map<int, list<int>> &toFreeminsByServiceId
    );
};
