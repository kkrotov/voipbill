#pragma once

#include "../classes/Thread.h"
#include "../classes/BDb.h"
#include "../classes/Repository.h"

class ThreadSave : public Thread {
protected:
    BDb db_calls;
    Repository repository;
public:
    void run();
public:
    ThreadSave();
    static const char* idName() { return "save"; }
};

