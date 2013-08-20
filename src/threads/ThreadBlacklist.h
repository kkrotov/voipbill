#pragma once

#include "Thread.h"
#include "../classes/DataLoader.h"
#include "../classes/BlackListLocal.h"
#include "../classes/BlackListGlobal.h"

class ThreadBlacklist : public Thread {
    Timer t;
    DataLoader *loader;
    BlackListLocal *blacklist_local;
    BlackListGlobal *blacklist_global;

    time_t last_sync_from_openca_time;

    void wait();
    void prepare();

    void run();

    void sync_blacklist();
    void sync_once_per_day();

    void update_voip_auto_disabled();


    void htmlfull(stringstream &html);
public:
    ThreadBlacklist();
};
