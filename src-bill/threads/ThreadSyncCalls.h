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

    int remain_count_to_update_aggr = 0;

    Repository repository;

    void run();
    bool getCurrentMonths(string &local_prev_sync_month, string &local_curr_sync_month, string &local_next_sync_month);
    bool copyCallsPart(string month, unsigned long limit);
    void htmlfull(stringstream &html);
    bool getLocalId (string month, long long &id, string &time);
    void getCentalId (string month, long long &id, string &time);

public:
    ThreadSyncCalls();
    static const char* idName() { return "sync_calls"; }
    bool hasFullHtml() override;

    void do_calls_aggr_update();
};
