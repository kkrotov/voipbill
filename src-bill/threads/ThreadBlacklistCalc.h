#pragma once

#include "../classes/Thread.h"
#include "../classes/Repository.h"
#include "../classes/BlackListLocal.h"
#include "../classes/BlackListGlobal.h"
#include "../classes/BlackListTrunk.h"
#include "../classes/Repository.h"

class ThreadBlacklistCalc : public Thread {

    Repository repository;

    BlackListLocal *blacklist_local;
    BlackListGlobal *blacklist_global;
    BlackListTrunk *blacklist_trunk;

    bool ready();

    void run();

public:
    ThreadBlacklistCalc();
    static const char* idName() { return "blacklist_calc"; }
};
