#include "RadiusAuthServer.h"

#include "../classes/Exception.h"
#include "RadiusAuthProcessor.h"

void RadiusAuthServer::run(string secret, uint16_t port) {

    RadiusServerStack l_stack(RadiusSecret(secret.c_str()), port);
    if (!l_stack.isValid()){
        throw Exception("Can not create RADIUS server stack");
    }

    RadiusPacket & l_request = l_stack.getRequest();
    while (true) {

        if (l_stack.receiveRequest() != RC_SUCCESS) {
            continue;
        }

        pLogMessage logRequest(new LogMessage());
        logRequest->type = "auth";


        try {
            RadiusAuthRequest request;
            RadiusAuthResponse response;

            response.timer.start();

            spawnRequest(l_stack.getRequest(), request, logRequest);
            processRequest(request, response, logRequest);

            response.timer.stop();

            sendResponse(l_stack, response, logRequest);

        } catch (Exception &e) {
            e.addTrace("RadiusAuthServer");
            Log::exception(e);
        } catch (std::exception &e) {
            Log::error("RadiusAuthServer: " + string(e.what()));
        } catch (...) {
            Log::error("RadiusAuthServer: ERROR");
        }

        Log::info(logRequest);
    }
}

void RadiusAuthServer::spawnRequest(RadiusPacket &p_request, RadiusAuthRequest &request, pLogMessage &logRequest)
{
    RadiusAttribute attr;

    char buffer[66000] = "";
    p_request.dump(buffer);
    last_request = string(buffer);
    last_response = "";
    last_error = "";
    request_count++;

    logRequest->message = "REQUEST:\n" + last_request;

    if (p_request.getCode() != D_PACKET_ACCESS_REQUEST) {
        throw Exception("Wrong request code");
    }

    for(p_request.getFirstAttribute(attr); attr.isValid(); p_request.getNextAttribute(attr)) {
        if (attr.getType() == D_ATTR_CALLING_STATION_ID) {

            readRequestString(attr, request.srcNumber);
            logRequest->params["req_calling"] = request.srcNumber;

        } else if (attr.getType() == D_ATTR_CALLED_STATION_ID) {

            readRequestString(attr, request.dstNumber);
            logRequest->params["req_called"] = request.dstNumber;

        } else if (attr.getType() == D_ATTR_VENDOR_SPECIFIC && attr.getVendorId() == 9 && attr.getVendorType() == 1) {

            string attrName;
            string attrValue;
            readRequestVendorString(attr, attrName, attrValue);

            if (attrName == "INCOMING-CALLING-ADDRESS-NOA") {
                request.srcNoa = atoi(attrValue.c_str());
            } else if (attrName == "INCOMING-CALLED-ADDRESS-NOA") {
                request.dstNoa = atoi(attrValue.c_str());
            } else if (attrName == "INCOMING-ROUTEID") {
                request.trunkName = attrValue;
                logRequest->params["req_trunk"] = request.trunkName;
            } else if (attrName == "INCOMING-REDIRECTING-ADDRESS") {
                request.redirectNumber = attrValue;
                logRequest->params["req_redirect"] = request.redirectNumber;
            }
        }
    }
}

void RadiusAuthServer::processRequest(RadiusAuthRequest &request, RadiusAuthResponse &response, pLogMessage &logRequest) {
    RadiusAuthProcessor processor(&request, &response, logRequest);
    processor.process();

}

void RadiusAuthServer::sendResponse(RadiusServerStack &p_stack, RadiusAuthResponse &response, pLogMessage &logRequest)
{
    RadiusPacket l_response(
        response.accept ? D_PACKET_ACCESS_ACCEPT : D_PACKET_ACCESS_REJECT,
        p_stack.getRequest()
    );
    logRequest->params["resp"] = response.accept ? "accept" : "reject";
    logRequest->params["duration"] = response.timer.tloop();

    if (response.srcNumber.size() > 0) {
        RadiusAttribute attr;
        l_response.addAttribute(D_ATTR_CALLING_STATION_ID, attr);
        attr.setString(response.srcNumber.c_str());
        logRequest->params["resp_calling"] = response.srcNumber;
    }

    if (response.dstNumber.size() > 0) {
        RadiusAttribute attr;
        l_response.addAttribute(D_ATTR_CALLED_STATION_ID, attr);
        attr.setString(response.dstNumber.c_str());
        logRequest->params["resp_calling"] = response.dstNumber;
    }

    if (response.routeCase.size() > 0) {
        RadiusAttribute attr;
        l_response.addAttribute(D_ATTR_VENDOR_SPECIFIC, attr);
        attr.setVendorId(9);
        attr.setVendorType(1);
        attr.setVendorString(("RTCASE=" + response.routeCase).c_str());
        logRequest->params["resp_rtcase"] = response.routeCase;
    }

    if (response.airp.size() > 0) {
        RadiusAttribute attr;
        l_response.addAttribute(D_ATTR_VENDOR_SPECIFIC, attr);
        attr.setVendorId(9);
        attr.setVendorType(1);
        attr.setVendorString(("AIRP=" + response.airp).c_str());
        logRequest->params["resp_airp"] = response.airp;
    }

    if (response.releaseReason.size() > 0) {
        RadiusAttribute attr;
        l_response.addAttribute(D_ATTR_VENDOR_SPECIFIC, attr);
        attr.setVendorId(9);
        attr.setVendorType(1);
        attr.setVendorString(("ReleaseReason=" + response.releaseReason).c_str());
        logRequest->params["resp_release_reason"] = response.releaseReason;
    }

    // ----------------------
    // send response packet
    // ----------------------
    p_stack.sendResponse(l_response);

    char buffer[66000] = "";
    l_response.dump(buffer);
    last_response = string(buffer);
    last_error = response.error;
    response_count++;

    logRequest->message += "RESPONSE:\n" + last_response;

    if (!last_error.empty()) {
        logRequest->message += "ERROR: " + last_error;
    }
}

void RadiusAuthServer::readRequestString(RadiusAttribute &attr, string &attrValue)
{
    const char * pData;
    uint16_t length;

    attr.getString(pData, length);

    char buffer[1024];
    strncpy(buffer, pData, length);
    buffer[length] = 0;

    attrValue = string(buffer);
}

void RadiusAuthServer::readRequestVendorString(RadiusAttribute &attr, string &attrName, string &attrValue)
{
    const char * pData;
    uint16_t length;

    attr.getVendorString(pData, length);

    char buffer[1024];
    strncpy(buffer, pData, length);
    buffer[length] = 0;

    char * pAttrValue = strchr(buffer, '=');
    if (pAttrValue != nullptr) {
        *pAttrValue = 0;
        pAttrValue++;
        attrName = string(buffer);
        attrValue = string(pAttrValue);
    }
}