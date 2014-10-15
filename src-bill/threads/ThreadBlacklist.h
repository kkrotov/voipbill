#pragma once

#include "../../src/threads/Thread.h"
#include "../classes/DataLoader.h"
#include "../classes/BlackListLocal.h"
#include "../classes/BlackListGlobal.h"
#include "../classes/BlackListTrunk.h"

class ThreadBlacklist : public Thread {
    DataLoader *loader;
    BlackListLocal *blacklist_local;
    BlackListGlobal *blacklist_global;
    BlackListTrunk *blacklist_trunk;

    time_t last_sync_from_openca_time;

    bool ready();
    bool prepare();

    void run();

    void sync_blacklist();
    void sync_once_per_hour();

    void update_voip_auto_disabled();


    void htmlfull(stringstream &html);
public:
    ThreadBlacklist();
    static const char* idName() { return "blacklist"; }
};
