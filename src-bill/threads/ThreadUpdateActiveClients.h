#pragma once

#include "../../src/threads/Thread.h"
#include "../data/DataContainer.h"
#include "../data/DataBillingContainer.h"

class ThreadUpdateActiveClients : public Thread {
    DataContainer *data;

    PreparedData preparedData;

    bool ready();

    void run();

public:
    ThreadUpdateActiveClients();
    static const char* idName() { return "update_active_clients"; }
};
