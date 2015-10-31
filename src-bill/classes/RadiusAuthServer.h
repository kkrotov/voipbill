#pragma once

#include "../common.h"
#include "../../libs/ace-radius/RadiusServerStack.h"

struct RadiusAuthRequest {
    uint8_t id;
    struct in_addr nasIp;
    string callId;
    string srcNumber;
    string dstNumber;
    string redirectNumber;
    string trunkName;
    int srcNoa = 3;
    int dstNoa = 3;
};

struct RadiusAuthResponse {
    uint8_t id;
    string srcNumber;
    string dstNumber;
    string routeCase;
    string releaseReason;
    string airp;
};

class RadiusAuthServer {
public:
    void run(string secret, uint16_t port);

private:
    void spawnRequest(RadiusPacket &p_request, RadiusAuthRequest &request);
    void processRequest(RadiusAuthRequest &request, RadiusAuthResponse &response);
    void sendResponse(RadiusServerStack &p_stack, RadiusAuthResponse &response);
    void readRequestString(RadiusAttribute &attr, string &attrValue);
    void readRequestVendorString(RadiusAttribute &attr, string &attrName, string &attrValue);
    int verifyRequest(RadiusPacket & p_request);
};

