#pragma once

#include "../../src/threads/Thread.h"
#include "../../src/classes/BDb.h"
#include "../data/DataBillingContainer.h"

class ThreadSaveCalls : public Thread {
protected:
    BDb db_calls;
    DataBillingContainer * billingData;
public:
    void run();
public:
    ThreadSaveCalls();
    static const char* idName() { return "save_calls"; }
};

