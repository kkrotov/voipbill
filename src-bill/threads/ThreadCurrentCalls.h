#pragma once

#include "../classes/Thread.h"
#include "../classes/Repository.h"

class ThreadCurrentCalls : public Thread {
public:
    BDb db_calls;
    Repository repository;

    void run();

public:
    ThreadCurrentCalls();
    static const char* idName() { return "current_calls"; }
};

