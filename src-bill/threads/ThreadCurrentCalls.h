#pragma once

#include "../../src/threads/Thread.h"
#include "../data/DataContainer.h"
#include "../data/DataBillingContainer.h"
#include "../data/DataCurrentCallsContainer.h"

class ThreadCurrentCalls : public Thread {
public:
    BDb db_calls;
    DataContainer * data;
    DataCurrentCallsContainer * currentCalls;

    long long int lastCallId = 0;
    int callPerSecond = 0;
    int maxCallPerSecond = 0;

    void run();


    void htmlfull(stringstream &html);

public:
    ThreadCurrentCalls();
    static const char* idName() { return "current_calls"; }
};

