#pragma once

#include "../classes/Thread.h"
#include "../../libs/ace-radius/RadiusServerStack.h"

class ThreadRadiusAuthServer : public Thread {

    void run();
    int verifyRequest(RadiusPacket & p_request);
    int sendResponse(RadiusServerStack &p_stack);

public:

    ThreadRadiusAuthServer();
    static const char* idName() { return "udp_server"; }
};
