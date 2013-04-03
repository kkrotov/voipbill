#pragma once

#include <list>
#include <mutex>
using namespace std;

#include "Conf.h"

class Thread;

class App
{
public:
    Conf conf;
    mutex threads_mutex;
    list<Thread*> threads;

    bool init_sync_done;
    bool init_load_counters_done;
    bool init_load_data_done;
    bool init_bill_runtime_started;

    App();

    bool init(int argc, char* argv[]);
    void run();

    void register_thread(Thread * thread);
    void unregister_thread(Thread * thread);

};

extern App app;


