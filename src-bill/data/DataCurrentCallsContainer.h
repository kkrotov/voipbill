#pragma once

#include "../classes/BDb.h"
#include "../common.h"
#include "../models/Cdr.h"
#include "../models/Call.h"
#include "../lists/StatsPackageCounter.h"
#include "../lists/ClientCounter.h"
#include "../lists/FminCounter.h"
#include "CurrentCdrData.h"

class DataCurrentCallsContainer {
public:
    CurrentCdrData currentCdr;

    long long int lastCallId = 0;
    int callPerSecond = -1;
    int maxCallPerSecond = -1;

    DataCurrentCallsContainer();

    static DataCurrentCallsContainer * instance();

    bool ready();

    void setCallsWaitingSaving(shared_ptr<vector<Call>> &newCallsWaitSaving);
    shared_ptr<vector<Call>> getCallsWaitingSaving();

    void setClientCounter(shared_ptr<ClientCounter> &newClientCounter);
    shared_ptr<ClientCounter> getClientCounter();

    void setFminCounter(shared_ptr<FminCounter> &newFminCounter);
    shared_ptr<FminCounter> getFminCounter();

    void setStatsPackagesCounter(shared_ptr<StatsPackageCounter> &newStatsPackagesCounter);
    shared_ptr<StatsPackageCounter> getStatsPackagesCounter();

private:
    Spinlock lock;
    shared_ptr<vector<Call>> callsWaitSaving;
    shared_ptr<StatsPackageCounter> statsPackagesCounter;
    shared_ptr<ClientCounter> clientCounter;
    shared_ptr<FminCounter> fminCounter;
};