#pragma once

#include "../../src/threads/Thread.h"
#include "../classes/DataLoader.h"

class ThreadSyncCounters : public Thread {
    DataLoader *loader;
    BDb db_main;

    bool ready();

    bool prepare();

    void run();

    void save_client_counters(bool clear = false);

    void htmlfull(stringstream &html);
public:
    ThreadSyncCounters();
};
