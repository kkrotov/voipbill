#pragma once

#include "../classes/Thread.h"
#include "../classes/Repository.h"

class ThreadUpdateActiveClients : public Thread {
    Repository repository;

    void run();

public:
    ThreadUpdateActiveClients();
    static const char* idName() { return "update_active_clients"; }
};
