#pragma once

#include "Thread.h"
#include "../classes/DataLoader.h"

class ThreadSaveCounters : public Thread
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
    ThreadSaveCounters();
};
