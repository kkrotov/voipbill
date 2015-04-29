#pragma once

#include "../../src/threads/Thread.h"
#include "../../src/classes/BDb.h"
#include "../../src/classes/Timer.h"
#include "../data/DataContainer.h"
#include "../data/DataBillingContainer.h"

class ThreadLoader : public Thread {
    DataContainer * data;
    DataBillingContainer * billingData;
    BDb db_calls;

    bool prepare();
    void run();

    void htmlfull(stringstream &html);

public:

    bool do_load_data(BDb *db = 0, DataContainer *data = 0);

    bool do_load_counters(BDb *db = 0, DataBillingContainer *billingData = 0);

    ThreadLoader();
    static const char* idName() { return "loader"; }
};
