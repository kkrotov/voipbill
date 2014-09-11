#pragma once

#include "../../src/threads/Thread.h"
#include "../../src/classes/BDb.h"

class ThreadSyncCalls : public Thread {
    BDb db_main;   
    BDb db_calls;

    void run();

    void htmlfull(stringstream &html);
public:
    ThreadSyncCalls();
};
