#pragma once

#include "../classes/Thread.h"
#include "../classes/Repository.h"

class ThreadSyncLocks : public Thread {
    BDb db_main;
    Repository repository;

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
