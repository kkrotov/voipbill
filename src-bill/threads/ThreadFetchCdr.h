#pragma once

#include "../classes/Thread.h"
#include "../classes/Repository.h"

class ThreadFetchCdr : public Thread {
protected:
    BDb db_calls;
    Repository repository;


public:
    bool ready();

    void run();
public:
    ThreadFetchCdr();
    static const char* idName() { return "fetch_cdr"; }
};

