#pragma once

#include "../../src/threads/Thread.h"
#include "../../src/classes/BDb.h"
#include <list>

#include "../../src/classes/Timer.h"

struct qsync;

class ThreadSync : public Thread {
    BDb db_main;
    BDb db_calls;
    list<qsync> syncs;
    int errors;

    unsigned long long time_loop;
    unsigned long long full_time_loop;

    bool sync_full(qsync &s);

    bool sync_notfull(qsync &s);

    bool prepare();
    void run();

    void htmlfull(stringstream &html);
public:

    ThreadSync();
    static const char* idName() { return "sync"; }

    bool do_sync();

};
