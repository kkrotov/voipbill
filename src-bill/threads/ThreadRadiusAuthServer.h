#pragma once

#include "../classes/Thread.h"
#include "../classes/RadiusAuthServer.h"

class ThreadRadiusAuthServer : public Thread {

    RadiusAuthServer server;

    void run();

    bool hasFullHtml() override;
    void htmlfull(stringstream &html);


public:

    ThreadRadiusAuthServer();
    static const char* idName() { return "radius_auth_server"; }
    int lastRequestTime () { return server.last_request_time; }
};
