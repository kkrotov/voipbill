#pragma once

#include "../classes/Thread.h"
#include "../classes/BDb.h"
#include "../classes/Billing.h"

class ThreadBillRuntime : public Thread {
protected:
    BDb db_calls;
    Billing billing;
    Repository repository;
public:
    void run();
public:
    ThreadBillRuntime();
    static const char* idName() { return "runtime"; }
};

