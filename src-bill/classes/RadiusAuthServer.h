#pragma once

#include "../common.h"
#include "../../libs/ace-radius/RadiusServerStack.h"
#include "../classes/Log.h"

struct RadiusAuthRequest {
    string trunkName;
    string srcNumber;
    string dstNumber;
    string redirectNumber;
    int srcNoa = 3;
    int dstNoa = 3;

    void dump(stringstream &trace) {
        trace << "(";
        trace << "trunk_name: " << trunkName << ", ";
        trace << "src_number: " << srcNumber << ", ";
        trace << "dst_number: " << dstNumber << ", ";
        trace << "redirect_number: " << redirectNumber << ", ";
        trace << "src_noa: " << srcNoa << ", ";
        trace << "dst_noa: " << dstNoa << ", ";
        trace << ")";
    }
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
        this->srcNumber = "";
        this->dstNumber = "";
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
    void spawnRequest(RadiusPacket &p_request, RadiusAuthRequest &request, pLogMessage &logRequest);
    void processRequest(RadiusAuthRequest &request, RadiusAuthResponse &response, pLogMessage &logRequest);
    void sendResponse(RadiusServerStack &p_stack, RadiusAuthResponse &response, pLogMessage &logRequest);
    void readRequestString(RadiusAttribute &attr, string &attrValue);
    void readRequestVendorString(RadiusAttribute &attr, string &attrName, string &attrValue);
};

