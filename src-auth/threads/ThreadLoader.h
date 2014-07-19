#pragma once

#include "../../src/threads/Thread.h"
#include "../../src/classes/BDb.h"
#include "../../src/classes/Timer.h"

class ThreadLoader : public Thread {
    BDb db_local;
    int errors;

    void run();

    void htmlfull(stringstream &html);

public:

    ThreadLoader();
};
