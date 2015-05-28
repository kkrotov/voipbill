#pragma once

#include "../../src/threads/Thread.h"
#include "../data/DataContainer.h"
#include "../data/DataBillingContainer.h"
#include "../classes/BlackListLocal.h"
#include "../classes/BlackListGlobal.h"
#include "../classes/BlackListTrunk.h"

class ThreadBlacklistCalc : public Thread {
    DataContainer *data;
    DataBillingContainer *billingData;

    PreparedData preparedData;

    BlackListLocal *blacklist_local;
    BlackListGlobal *blacklist_global;
    BlackListTrunk *blacklist_trunk;

    bool ready();

    void run();

public:
    ThreadBlacklistCalc();
    static const char* idName() { return "blacklist_calc"; }
};
