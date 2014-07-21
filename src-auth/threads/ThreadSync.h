#pragma once

#include "../../src/threads/Thread.h"
#include "../../src/classes/BDb.h"
#include <list>

#include "../../src/classes/Timer.h"

struct qsync;

class ThreadSync : public Thread {
    BDb db_main;
    BDb db_local;
    list<qsync> syncs;
    int errors;

    unsigned long long time_loop;
    unsigned long long full_time_loop;

    void run();

    int getNewConfigVersionId();

    void htmlfull(stringstream &html);
public:

    ThreadSync();

};
