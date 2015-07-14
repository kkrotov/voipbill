#pragma once

#include "../common.h"
#include "Spinlock.h"
#include "BDb.h"
#include "../models/StatsFreemin.h"
#include "../models/Call.h"

class StatsFreeminManager
{
private:
    int lastStatsFreeminId = 0;
public:
    long long int lastStoredCallTime;

    Spinlock lock;

    map<int, StatsFreemin> storedStatsFreemin;
    map<int, list<int>> storedFreeminsByServiceId;

    map<int, StatsFreemin> realtimeStatsFreemin;
    map<int, list<int>> realtimeFreeminsByServiceId;

    map<int, StatsFreemin> tmpStatsFreemin;
    map<int, list<int>> tmpFreeminsByServiceId;

    void load(BDb * db);
    int getSeconds(Call * call);
    void add(Call * call);
    size_t size();

    void moveRealtimeToTemp();
    void moveTempToStored();

private:
    int getSeconds(Call * call, map<int, list<int>> &freeminsByServiceId, map<int, StatsFreemin> &statsFreemin);
    void createStatsFreemin(Call * call);
    bool updateStatsFreemin(Call * call);
    void move(
            map<int, StatsFreemin> &fromStatsFreemin, map<int, list<int>> &fromFreeminsByServiceId,
            map<int, StatsFreemin> &toStatsFreemin, map<int, list<int>> &toFreeminsByServiceId
    );
};
