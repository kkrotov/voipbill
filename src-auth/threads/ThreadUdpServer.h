#pragma once

#include "../../src/threads/Thread.h"
#include "../../src/classes/BDb.h"
#include "../../src/classes/Timer.h"

class ThreadUdpServer : public Thread {
    int errors;

    void run();

    void htmlfull(stringstream &html);

public:

    ThreadUdpServer();
};
