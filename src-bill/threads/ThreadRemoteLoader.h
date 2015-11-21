#pragma once

#include "../classes/Thread.h"
#include "../classes/Spinlock.h"
#include "../lists/GlobalCountersList.h"
#include "../classes/Repository.h"

class ThreadRemoteLoader : public Thread {
private:
    Repository repository;
    BDb db_main;

    void run();

public:
    ThreadRemoteLoader();
    static const char* idName() { return "remote_loader"; }
};

