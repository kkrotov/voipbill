#pragma once

#include "../common.h"
#include "../../libs/ace-radius/RadiusServerStack.h"

struct RadiusAuthRequest {
    string srcNumber;
    string dstNumber;
    string redirectNumber;
    string trunkName;
    int srcNoa = 3;
    int dstNoa = 3;
};

struct RadiusAuthResponse {
    bool accept;
    string srcNumber;
    string dstNumber;
    string routeCase;
    string releaseReason;
    string airp;
    string error;

    void setRouteCase(string routeCase) {
        this->accept = true;
        this->routeCase = routeCase;
    }

    void setAirp(string airp) {
        this->accept = true;
        this->airp = airp;
    }

    void setReleaseReason(string releaseReason) {
        this->accept = true;
        this->releaseReason = releaseReason;
    }

    void setAccept() {
        this->accept = true;
    }

    void setReject() {
        this->accept = false;
    }
};

class RadiusAuthServer {
public:
    size_t request_count = 0;
    size_t response_count = 0;
    string last_request;
    string last_response;
    string last_error;

    void run(string secret, uint16_t port);

private:
    void spawnRequest(RadiusPacket &p_request, RadiusAuthRequest &request);
    void processRequest(RadiusAuthRequest &request, RadiusAuthResponse &response);
    void sendResponse(RadiusServerStack &p_stack, RadiusAuthResponse &response);
    void readRequestString(RadiusAttribute &attr, string &attrValue);
    void readRequestVendorString(RadiusAttribute &attr, string &attrName, string &attrValue);
};

