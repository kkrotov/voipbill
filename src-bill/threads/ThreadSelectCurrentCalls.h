#pragma once

#include "../../src/threads/Thread.h"
#include "../data/DataContainer.h"

class ThreadSelectCurrentCalls : public Thread {
public:
    BDb db_calls;
    DataContainer * data;

    void run();


    void htmlfull(stringstream &html);

public:
    ThreadSelectCurrentCalls();
    static const char* idName() { return "currentcalls"; }
};

