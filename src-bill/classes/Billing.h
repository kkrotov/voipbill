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
    void updateClientCounters(CallInfo &callInfo, ClientCounter * clientCounter);
    void updateFreeMinsCounters(CallInfo &callInfo, FminCounter * fminCounter);
    void updatePackageStats(CallInfo &callInfo, StatsPackageCounter * statsPackageCounter);
};

