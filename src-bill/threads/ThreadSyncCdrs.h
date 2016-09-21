#pragma once

#include "../classes/Thread.h"
#include "../classes/BDb.h"
#include "../classes/Repository.h"

class ThreadSyncCdrs : public Thread {

    BDb db_main;   
    BDb db_calls;
    Repository repository;

    void run();
    bool getCurrentMonths (string relname, string fieldname, long long int id, string &local_prev_sync_month, string &local_curr_sync_month, string &local_next_sync_month);
    bool getCurrentMonths (string &local_prev_sync_month, string &local_curr_sync_month, string &local_next_sync_month);
    bool syncCallsCdr();
    bool copyCallsCdr(string month, int limit);
    bool copyDblinkCallsCdr(string month);
    bool syncCallsCdrUnfinished();
    bool copyCallsCdrUnfinished(string month, int limit);
    bool copyDblinkCallsCdrUnfinished(string local_curr_sync_month);

public:
    ThreadSyncCdrs();
    static const char* idName() { return "sync_cdrs"; }
};
