#pragma once

#include "../classes/Thread.h"
#include "../classes/Repository.h"


class ThreadLimitControl : public Thread {
protected:
    Repository repository;

    bool ready();

    void run();

    bool limitControlKillNeeded(Call &call);
    void fetchGlobalCounters(int accountId, double &globalBalanceSum, double &globalDaySum, double &globalMonthSum, double vat_rate);

public:
    ThreadLimitControl();
    static const char* idName() { return "limitcontrol"; }
};

