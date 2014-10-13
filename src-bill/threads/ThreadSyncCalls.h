#pragma once

#include "../../src/threads/Thread.h"
#include "../../src/classes/BDb.h"

class ThreadSyncCalls : public Thread {
    BDb db_main;   
    BDb db_calls;
    
    string main_last_id;
    string main_last_time;

    void run();
    
    void copyCallsPart(string month);

    void htmlfull(stringstream &html);
public:
    ThreadSyncCalls();
};
