#pragma once

#include "../../src/threads/Thread.h"
#include "../data/DataContainer.h"
#include "../data/DataBillingContainer.h"
#include "../classes/BlackListLocal.h"
#include "../classes/BlackListGlobal.h"
#include "../classes/BlackListTrunk.h"

class ThreadBlacklistFetch : public Thread {

    BlackListLocal *blacklist_local;
    BlackListGlobal *blacklist_global;
    BlackListTrunk *blacklist_trunk;

    bool prepare();

    void run();

public:
    ThreadBlacklistFetch();
    static const char* idName() { return "blacklist_fetch"; }
};
