#pragma once

#include "../../src/threads/Thread.h"
#include "../data/DataBillingContainer.h"

class ThreadSyncCounters : public Thread {
    DataBillingContainer *billingData;
    BDb db_main;

    bool ready();

    void run();

    void save_client_counters();

    void htmlfull(stringstream &html);
public:
    ThreadSyncCounters();
    static const char* idName() { return "sync_counters"; }
};
