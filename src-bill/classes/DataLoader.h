#pragma once

#include <mutex>

#include "../lists/ClientCounter.h"
#include "../lists/FminCounter.h"


#include "../../src/classes/Loader.h"
#include "../classes/CounterLoader.h"

struct curr_data {
    time_t day;

    shared_ptr<ClientCounter> counter_client;
    shared_ptr<FminCounter> counter_fmin;
};

class DataLoader {
public:
    mutex counter_rwlock;
    shared_ptr<ClientCounter> counter_client;
    Loader<FminCounter> counter_fmin;

    DataLoader();

    static DataLoader * instance() {
        static DataLoader inst;
        return &inst;
    }

    bool get(DT & dt, curr_data & data, bool countersAlreadyLocked = false);

    bool load(BDb *db, DT & dt, curr_data & data, bool countersAlreadyLocked = false);

    bool ready(DT & dt, curr_data & data);

    void counter_append(CounterLoader & loader2);

};

