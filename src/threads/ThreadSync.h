#pragma once

#include "Thread.h"
#include "../classes/BDb.h"
#include <list>

#include "../classes/Timer.h"

struct qsync;

class ThreadSync : public Thread {
    Timer t;
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

    bool do_sync();

};
