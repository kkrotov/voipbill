#include "RadiusAuthServer.h"

#include "../classes/Exception.h"
#include "../classes/Log.h"
#include "RadiusAuthProcessor.h"

void RadiusAuthServer::run(string secret, uint16_t port) {

    RadiusServerStack l_stack(RadiusSecret(secret.c_str()), port);
    if (!l_stack.isValid()){
        throw Exception("Can not create RADIUS server stack");
    }

    RadiusPacket & l_request = l_stack.getRequest();
    while (true) {
        try {

            if (l_stack.receiveRequest() != RC_SUCCESS) {
                continue;
            }

            RadiusAuthRequest request;
            RadiusAuthResponse response;
            spawnRequest(l_stack.getRequest(), request);
            processRequest(request, response);

            sendResponse(l_stack, response);

        } catch (Exception &e) {
            e.addTrace("RadiusAuthServer");
            Log::exception(e);
        } catch (std::exception &e) {
            Log::error("RadiusAuthServer: " + string(e.what()));
        } catch (...) {
            Log::error("RadiusAuthServer: ERROR");
        }
    }
}

void RadiusAuthServer::spawnRequest(RadiusPacket &p_request, RadiusAuthRequest &request)
{
    RadiusAttribute attr;

    if (p_request.getCode() != D_PACKET_ACCESS_REQUEST) {
        throw Exception("Wrong request code");
    }

    request_count++;

    request.id = p_request.getID();

    for(p_request.getFirstAttribute(attr); attr.isValid(); p_request.getNextAttribute(attr)) {
        if (attr.getType() == D_ATTR_CALLING_STATION_ID) {

            request.nasIp = attr.getIPAddress();

        } else if (attr.getType() == D_ATTR_ACCT_SESSION_ID) {

            readRequestString(attr, request.callId);

        } else if (attr.getType() == D_ATTR_CALLING_STATION_ID) {

            readRequestString(attr, request.srcNumber);

        } else if (attr.getType() == D_ATTR_CALLED_STATION_ID) {

            readRequestString(attr, request.dstNumber);

        } else if (attr.getType() == D_ATTR_VENDOR_SPECIFIC && attr.getVendorId() == 9 && attr.getVendorType() == 1) {

            string attrName;
            string attrValue;
            readRequestVendorString(attr, attrName, attrValue);

            if (attrName == "INCOMING-CALLING-ADDRESS-NOA") {
                request.srcNoa = atoi(attrValue.c_str());
            } else if (attrName == "INCOMING-CALLED-ADDRESS-NOA") {
                request.dstNoa = atoi(attrValue.c_str());
            } else if (attrName == "INCOMING-ROUTEID=MCN_Transit_99") {
                request.trunkName = attrValue;
            }
        }
    }
}

void RadiusAuthServer::processRequest(RadiusAuthRequest &request, RadiusAuthResponse &response) {
    response.id = request.id;
    response.routeCase = "RU_Beeline_sip";

    RadiusAuthProcessor processor;
    processor.process(request, response);

}

void RadiusAuthServer::sendResponse(RadiusServerStack &p_stack, RadiusAuthResponse &response)
{
    RadiusPacket l_response(
        response.accept ? D_PACKET_ACCESS_ACCEPT : D_PACKET_ACCESS_REJECT,
        p_stack.getRequest()
    );

    if (response.srcNumber.size() > 0) {
        RadiusAttribute attr;
        l_response.addAttribute(D_ATTR_CALLING_STATION_ID, attr);
        attr.setString(response.srcNumber.c_str());
    }

    if (response.dstNumber.size() > 0) {
        RadiusAttribute attr;
        l_response.addAttribute(D_ATTR_CALLED_STATION_ID, attr);
        attr.setString(response.dstNumber.c_str());
    }

    if (response.routeCase.size() > 0) {
        RadiusAttribute attr;
        l_response.addAttribute(D_ATTR_VENDOR_SPECIFIC, attr);
        attr.setVendorId(9);
        attr.setVendorType(1);
        attr.setVendorString(("RTCASE=" + response.routeCase).c_str());
    }

    if (response.airp.size() > 0) {
        RadiusAttribute attr;
        l_response.addAttribute(D_ATTR_VENDOR_SPECIFIC, attr);
        attr.setVendorId(9);
        attr.setVendorType(1);
        attr.setVendorString(("AIRP=" + response.routeCase).c_str());
    }

    if (response.releaseReason.size() > 0) {
        RadiusAttribute attr;
        l_response.addAttribute(D_ATTR_VENDOR_SPECIFIC, attr);
        attr.setVendorId(9);
        attr.setVendorType(1);
        attr.setVendorString(("ReleaseReason" + response.releaseReason).c_str());
    }

    // ----------------------
    // send response packet
    // ----------------------
    p_stack.sendResponse(l_response);

    response_count++;
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

    char * pAttrName = &buffer[0];
    char * pAttrValue = strchr(pAttrName, '=');
    if (pAttrValue != nullptr) {
        *pAttrValue = 0;
        pAttrValue++;
        attrName = string(pAttrValue);
        attrValue = string(pAttrValue);
    }
}