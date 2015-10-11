#pragma once

#include "../classes/Thread.h"

class ThreadUdpServer : public Thread {

    void run();

public:

    ThreadUdpServer();
    static const char* idName() { return "udp_server"; }
};
