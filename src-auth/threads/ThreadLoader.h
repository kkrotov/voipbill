#pragma once

#include "../../src/threads/Thread.h"
#include "../../src/classes/BDb.h"
#include "../../src/classes/Timer.h"

class ThreadLoader : public Thread {
    int errors;

    bool ready();
    bool prepare();
    void run();

    void htmlfull(stringstream &html);

public:

    ThreadLoader();
};
