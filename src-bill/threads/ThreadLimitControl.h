#pragma once

#include "../../src/threads/Thread.h"
#include "../data/DataContainer.h"
#include "../data/DataBillingContainer.h"


class ThreadLimitControl : public Thread {
protected:
    PreparedData preparedData;

    bool ready();

    void run();

    bool limitControlKillNeeded(Call &call);
    void fetchGlobalCounters(int accountId, double &globalBalanceSum, double &globalDaySum, double &globalMonthSum, double vat_rate);

public:
    ThreadLimitControl();
    static const char* idName() { return "limitcontrol"; }
};

