#pragma once

#include "../classes/Thread.h"
#include "../classes/RadiusAuthServer.h"

class ThreadRadiusAuthServer : public Thread {

    RadiusAuthServer server;

    void run();

    void htmlfull(stringstream &html);


public:

    ThreadRadiusAuthServer();
    static const char* idName() { return "radius_auth_server"; }
};
