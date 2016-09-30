#pragma once

#include "../classes/Thread.h"
#include "../classes/Repository.h"

class ThreadSyncCounters : public Thread {
    BDb db_main;
    BDb db_calls;

    Repository repository;

    size_t last_sync_account_count;
    size_t last_sync_freemin_count;
    size_t last_sync_package_count;
    size_t last_sync_trunk_settings_count;
    size_t last_sync_nnp_package_minute_count;

    size_t total_sync_account_count;
    size_t total_sync_freemin_count;
    size_t total_sync_package_count;
    size_t total_sync_trunk_settings_count;
    size_t total_sync_nnp_package_minute_count;

    bool ready();

    void run();

    void htmlfull(stringstream &html);


public:
    bool hasFullHtml() override;

public:
    ThreadSyncCounters();
    static const char* idName() { return "sync_counters"; }
};
