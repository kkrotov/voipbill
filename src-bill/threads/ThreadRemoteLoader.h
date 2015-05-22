#pragma once

#include "../../src/threads/Thread.h"
#include "../../src/classes/Spinlock.h"
#include "../lists/GlobalCountersList.h"
#include "../data/DataContainer.h"

class ThreadRemoteLoader : public Thread {
private:
    DataContainer *data;
    BDb db_main;

    void run();

public:
    ThreadRemoteLoader();
    static const char* idName() { return "remote_loader"; }
};

