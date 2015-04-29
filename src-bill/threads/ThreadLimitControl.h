#pragma once

#include "../../src/threads/Thread.h"
#include "../data/DataContainer.h"
#include "../data/DataBillingContainer.h"


class ThreadLimitControl : public Thread {
protected:
    PreparedData preparedData;

    bool ready();

    void run();

    void htmlfull(stringstream &html);

    bool limitControlKillNeeded(Call &call);
    void fetchGlobalCounters(int accountId, double &globalBalanceSum, double &globalDaySum, double &globalMonthSum);

public:
    ThreadLimitControl();
    static const char* idName() { return "limitcontrol"; }
};

