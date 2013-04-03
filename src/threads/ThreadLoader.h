#pragma once

#include "../classes/BDb.h"
#include "Thread.h"
#include "../classes/DataLoader.h"
#include "../classes/Timer.h"



class ThreadLoader : public Thread
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

    ThreadLoader();
};
