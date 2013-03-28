#pragma once

#include "Loader.h"
#include "../lists/ClientCounter.h"
#include "../lists/FminCounter.h"

class CounterLoader
{
public:
    shared_ptr<ClientCounter> client;
    Loader<FminCounter> fmin;

    CounterLoader()
    {
        client = shared_ptr<ClientCounter>(new ClientCounter());
        fmin.need_load = false;
    }

    void clear(){
        client->clear();
        fmin.clear();
    }
};

