#pragma once

#include "../classes/Thread.h"
#include "../classes/BDb.h"
#include "../classes/Repository.h"

class ThreadSyncCalls : public Thread {
    BDb db_main;   
    BDb db_calls;
    string last_central_month;
    string last_central_time;
    long long int last_central_id;
    string last_local_month;
    string last_local_time;
    long long int last_local_id;

    Repository repository;

    void run();

    bool getCurrentMonths (string relname, string fieldname, long long int id, string &local_prev_sync_month, string &local_curr_sync_month, string &local_next_sync_month);
    bool getCurrentMonths (string &local_prev_sync_month, string &local_curr_sync_month, string &local_next_sync_month);
    bool syncCallsRaw();
    bool syncCallsCdr();
    bool syncCallsCdrUnfinished();
    bool copyCallsPart(string month);
    void htmlfull(stringstream &html);

public:
    ThreadSyncCalls();
    static const char* idName() { return "sync_calls"; }
    bool hasFullHtml() override;
};
