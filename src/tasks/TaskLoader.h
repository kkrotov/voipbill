#pragma once

#include "../BDb.h"
#include "Task.h"
#include "../DataLoader.h"
#include "../Timer.h"



class TaskLoader : public Task
{
    Timer t;
    DataLoader * loader;
    BDb db_calls;
    int errors;

    void wait();
    void prepare();
    void run();

    void htmlfull(stringstream &html);

public:

    bool do_load_data();

    bool do_load_counters();

    TaskLoader();
};
