#pragma once

#include "../classes/Thread.h"
#include "../classes/BlackListLocal.h"
#include "../classes/BlackListGlobal.h"
#include "../classes/BlackListTrunk.h"
#include "../classes/BlackListAntiFraudDisable.h"

class ThreadBlacklistFetch : public Thread {

    BlackListLocal *blacklist_local;
    BlackListGlobal *blacklist_global;
    BlackListTrunk *blacklist_trunk;
    BlackListAntiFraudDisable *blacklist_anti_fraud_disable;

    void run();

public:
    ThreadBlacklistFetch();
    static const char* idName() { return "blacklist_fetch"; }
};
