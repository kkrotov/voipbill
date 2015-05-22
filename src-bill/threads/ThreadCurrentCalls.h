#pragma once

#include "../../src/threads/Thread.h"
#include "../data/DataCurrentCallsContainer.h"

class ThreadCurrentCalls : public Thread {
public:
    BDb db_calls;
    DataCurrentCallsContainer * currentCalls;

    void run();

public:
    ThreadCurrentCalls();
    static const char* idName() { return "current_calls"; }
};

