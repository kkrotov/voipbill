#pragma once

#include "../classes/BDb.h"
#include "../classes/StatsAccountManager.h"
#include "../classes/StatsFreeminManager.h"
#include "../classes/StatsPackageManager.h"
#include "../classes/StatsTrunkSettingsManager.h"
#include "../classes/StatsNNPPackageMinuteManager.h"
#include "../common.h"
#include "../models/Cdr.h"
#include "../models/Call.h"
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

    void setStatsAccount(shared_ptr<StatsAccountManager> &newStatsAccount);
    shared_ptr<StatsAccountManager> getStatsAccount();

    void setStatsFreemin(shared_ptr<StatsFreeminManager> &newStatsFreemin);
    shared_ptr<StatsFreeminManager> getStatsFreemin();

    void setStatsPackage(shared_ptr<StatsPackageManager> &newStatsPackage);
    shared_ptr<StatsPackageManager> getStatsPackage();

    void setStatsTrunkSettings(shared_ptr<StatsTrunkSettingsManager> &newStatsTrunkSettings);
    shared_ptr<StatsTrunkSettingsManager> getStatsTrunkSettings();

    void setStatsNNPPackageMinute(shared_ptr<StatsNNPPackageMinuteManager> &newStatsNNPPackageMinute);
    shared_ptr<StatsNNPPackageMinuteManager> getStatsNNPPackageMinute();

private:
    Spinlock lock;
    shared_ptr<vector<Call>> callsWaitSaving;
    shared_ptr<StatsAccountManager> statsAccount;
    shared_ptr<StatsFreeminManager> statsFreemin;
    shared_ptr<StatsPackageManager> statsPackage;
    shared_ptr<StatsTrunkSettingsManager> statsTrunkSettings;
    shared_ptr<StatsNNPPackageMinuteManager> statsNNPPackageMinute;
};