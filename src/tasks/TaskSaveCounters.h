#pragma once

#include "Task.h"
#include "../DataLoader.h"

class TaskSaveCounters : public Task
{
    Timer t;
    DataLoader *loader;
    BDb db_main;
    BDb db_calls;

    void wait();

    void prepare();

    void run();

    bool save_client_counters(bool clear = false);

    bool save_calls();

    void htmlfull(stringstream &html);
public:
    TaskSaveCounters();
};
