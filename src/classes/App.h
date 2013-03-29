#pragma once

#include <list>
using namespace std;

#include <boost/interprocess/sync/interprocess_mutex.hpp>

#include "Conf.h"

class Task;

class App
{
public:
    Conf conf;
    boost::interprocess::interprocess_mutex tasks_mutex;
    list<Task*> tasks;

    bool init_sync_done;
    bool init_load_counters_done;
    bool init_load_data_done;
    bool init_bill_runtime_started;

    App();

    bool init(int argc, char* argv[]);
    void run();

    void register_task(Task * task);
    void unregister_task(Task * task);

};

extern App app;


