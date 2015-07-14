#pragma once

#include "../classes/Thread.h"
#include "../classes/Repository.h"
#include "../classes/CdrLoader.h"

class ThreadFetchCdr : public Thread {
protected:
    BDb db_calls;
    Repository repository;
    CdrLoader cdrLoader;


public:
    bool ready();

    void run();
public:
    ThreadFetchCdr();
    static const char* idName() { return "fetch_cdr"; }
};

