#pragma once

#include "../../src/threads/Thread.h"
#include "../../src/classes/BDb.h"
#include <list>

#include "../../src/classes/Timer.h"
#include "../sync/ManagerPull.h"

class ThreadSync : public Thread {
    ManagerPull manager;

    bool prepare();
    void run();

    void htmlfull(stringstream &html);
public:

    ThreadSync();
    static const char* idName() { return "sync"; }

};
