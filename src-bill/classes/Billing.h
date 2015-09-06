#pragma once

#include "Repository.h"

class Billing {
public:
    Repository repository;

public:
    void setData(DataContainer *data);
    void setBillingData(DataBillingContainer *billingData);

    void calcCurrentCalls();
    void calc();
private:
    void updateClientCounters(CallInfo &callInfo, StatsAccountManager *statsAccount);
    void updateFreeMinsCounters(CallInfo &callInfo, StatsFreeminManager *statsFreemin);
    void updatePackageStats(CallInfo &callInfo, StatsPackageManager *statsPackage);
};

