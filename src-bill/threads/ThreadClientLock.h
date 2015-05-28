#pragma once

#include "../../src/threads/Thread.h"
#include "../data/DataContainer.h"
#include "../data/DataBillingContainer.h"

class ThreadClientLock : public Thread {
    DataContainer *data;
    DataBillingContainer *billingData;

    PreparedData preparedData;

    bool ready();

    void run();

    bool needLockLocal(ClientCounterObj &cc, int client_account_id);
    bool needLockGlobal(ClientCounterObj &cc, int client_account_id);

public:
    ThreadClientLock();
    static const char* idName() { return "client_lock"; }
};
