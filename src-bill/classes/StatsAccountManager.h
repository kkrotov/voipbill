#pragma once

#include "../common.h"
#include "BDb.h"
#include "Spinlock.h"
#include "../models/StatsAccount.h"

class StatsAccountManager
{
private:
    long long int lastStoredCallTime;

    Spinlock lock;

    map<int, StatsAccount> storedStatsAccount;
    map<int, StatsAccount> realtimeStatsAccount;
    map<int, StatsAccount> tmpStatsAccount;


public:
    void load(BDb * db);
    void reload(BDb * db);

    size_t size();

    void moveRealtimeToTemp();
    void moveTempToStored();

private:
    void move(map<int, StatsAccount> &fromStatsAccount, map<int, StatsAccount> &toStatsAccount);
};