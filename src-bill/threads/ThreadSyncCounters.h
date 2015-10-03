#pragma once

#include "../classes/Thread.h"
#include "../classes/Repository.h"

class ThreadSyncCounters : public Thread {
    BDb db_main;

    Repository repository;

    size_t last_sync_account_count;
    size_t last_sync_freemin_count;
    size_t last_sync_package_count;
    size_t total_sync_account_count;
    size_t total_sync_freemin_count;
    size_t total_sync_package_count;

    bool ready();

    void run();

    void do_sync_account();
    void do_sync_freemin();
    void do_sync_package();

    void htmlfull(stringstream &html);


public:
    bool hasFullHtml() override;

public:
    ThreadSyncCounters();
    static const char* idName() { return "sync_counters"; }
};
