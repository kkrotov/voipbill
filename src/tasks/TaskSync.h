#pragma once

#include "Task.h"
#include "../BDb.h"
#include <list>

#include "../Timer.h"

struct qsync;

class TaskSync : public Task
{
    Timer t;
    BDb db_main;
    BDb db_calls;
    list<qsync> syncs;
    int errors;

    unsigned long long time_loop;
    unsigned long long full_time_loop;

    bool sync_full(qsync &s);

    bool sync_notfull(qsync &s);

    void prepare();
    void run();

    void htmlfull(stringstream &html);
public:

    TaskSync();

    bool do_sync();

};
