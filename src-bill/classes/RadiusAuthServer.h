#pragma once

#include <map> 
#include "../common.h"
#include "../../libs/ace-radius/RadiusServerStack.h"
#include "../classes/Log.h"
#include "../classes/Timer.h"
#include "RadiusAuthProcessor.h"
#include "RadiusAuthRequestResponse.h"


class RadiusAuthServer {

    std::map<int, std::pair<RejectReason, time_t> > accountIdsBlockedBefore;
public:
    size_t request_count = 0;
    size_t response_count = 0;
    string last_request;
    time_t last_request_time = 0;
    string last_response;
    time_t last_response_time = 0;
    string last_error;

    void run(string secret, uint16_t port);

private:
    void spawnRequest(RadiusPacket &p_request, RadiusAuthRequest &request, pLogMessage &logRequest);
    void processRequest(RadiusAuthRequest &request, RadiusAuthResponse &response, pLogMessage &logRequest);
    void sendResponse(RadiusServerStack &p_stack, RadiusAuthResponse &response, pLogMessage &logRequest);
    void readRequestString(RadiusAttribute &attr, string &attrValue);
    void readRequestVendorString(RadiusAttribute &attr, string &attrName, string &attrValue);
};

