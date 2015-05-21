#pragma once

#include "../../src/threads/Thread.h"
#include "../data/DataBillingContainer.h"

class ThreadSyncCounters : public Thread {
    DataBillingContainer *billingData;
    BDb db_main;
    int last_sync_count;
    int total_sync_count;

    bool ready();

    void run();

    void save_client_counters();

    void htmlfull(stringstream &html);
public:
    ThreadSyncCounters();
    static const char* idName() { return "sync_counters"; }
};
