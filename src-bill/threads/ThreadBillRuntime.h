#pragma once

#include "../../src/threads/Thread.h"
#include "../../src/classes/BDb.h"
#include "../classes/Billing.h"

class ThreadBillRuntime : public Thread {
protected:
    BDb db_calls;
    Billing billing;
    DataBillingContainer *billingData;
public:
    void run();

    void htmlfull(stringstream &html);
public:
    ThreadBillRuntime();
    static const char* idName() { return "runtime"; }
};

