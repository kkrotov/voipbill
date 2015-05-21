#pragma once

#include "../../src/threads/Thread.h"
#include "../../src/classes/BDb.h"
#include "../data/DataBillingContainer.h"

class ThreadSyncCalls : public Thread {
    BDb db_main;   
    BDb db_calls;

    DataBillingContainer * billingData;
    
    void run();
    
    void copyCallsPart(string month);

    void htmlfull(stringstream &html);
public:
    ThreadSyncCalls();
    static const char* idName() { return "sync_calls"; }
};
