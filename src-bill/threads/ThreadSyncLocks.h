#pragma once

#include "../../src/threads/Thread.h"
#include "../data/DataBillingContainer.h"

class ThreadSyncLocks : public Thread {
    BDb db_main;

    DataBillingContainer * billingData;

    int last_sync_count;
    int total_sync_count;

    bool ready();

    void run();

    void save_client_locks();

    void htmlfull(stringstream &html);


public:
    bool hasFullHtml() override;

public:
    ThreadSyncLocks();
    static const char* idName() { return "sync_locks"; }
};
