#pragma once

#include "../classes/Thread.h"
#include "../classes/BDb.h"
#include "../classes/Repository.h"

class ThreadSyncCalls : public Thread {
    BDb db_main;   
    BDb db_calls;

    Repository repository;

    void run();
    
    void copyCallsPart(string month);
public:
    ThreadSyncCalls();
    static const char* idName() { return "sync_calls"; }
};
