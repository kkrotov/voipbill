#pragma once

#include "../../src/threads/Thread.h"
#include "../data/DataContainer.h"
#include "../data/DataBillingContainer.h"

class ThreadCurrentCalls : public Thread {
public:
    BDb db_calls;
    DataContainer * data;
    DataBillingContainer * billingData;
    DataBillingContainer * billingDataCurrentCalls;

    void run();


    void htmlfull(stringstream &html);

public:
    ThreadCurrentCalls();
    static const char* idName() { return "current_calls"; }
};

