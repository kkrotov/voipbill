#include "ThreadRadiusAuthServer.h"
#include "../classes/AppBill.h"
#include "../classes/UdpMessageProcessor.h"

void ThreadRadiusAuthServer::run() {
    // create instance of server stack
    RadiusServerStack l_stack(RadiusSecret(app().conf.radius_secret.c_str()));
    if (!l_stack.isValid())
    {
        Log::error("Can not create RADIUS server stack\n");
        return;
    }

    // reference to request packet
    RadiusPacket & l_request = l_stack.getRequest();
    while (1)
    {
        int l_rc = l_stack.receiveRequest();
        if (l_rc == RC_SUCCESS)
        {
            if (verifyRequest(l_stack.getRequest()) == RC_SUCCESS)
                sendResponse(l_stack);
        }
    }
}

int ThreadRadiusAuthServer::verifyRequest(RadiusPacket & p_request)
{
    RadiusAttribute l_attr;
    const char * l_data;
    uint16_t l_length;
    char l_password[D_USER_PASSWORD_MAX_LENGTH];

    // answer only ACCESS_REQUEST packets
    if (p_request.getCode() != D_PACKET_ACCESS_REQUEST)
    {
        printf("ERROR: Wrong request code\n");
        return RC_FAIL;
    }

    // verify User-Name attribute
    if (p_request.findAttribute(D_ATTR_USER_NAME, l_attr) != RC_SUCCESS)
    {
        printf("ERROR: Can't find User-Name attribute\n");
        return RC_FAIL;
    }

    if (l_attr.getString(l_data, l_length) != RC_SUCCESS)
    {
        printf("ERROR: Can't parse User-Name attribute\n");
        return RC_FAIL;
    }

//    if ((l_length != 4) || (memcmp(l_data, "nemo", 4) != 0))
//    {
//        printf("ERROR: Wrong User-Name attribute value\n");
//        return RC_FAIL;
//    }

    // verify User-Password attribute
    if (p_request.findAttribute(D_ATTR_USER_PASSWORD, l_attr) != RC_SUCCESS)
    {
        printf("ERROR: Can't find User-Password attribute\n");
        return RC_FAIL;
    }

    if (l_attr.getUserPassword(l_password, l_length) != RC_SUCCESS)
    {
        printf("ERROR: Can't parse User-Password attribute\n");
        return RC_FAIL;
    }

//    if ((l_length != 10) || (memcmp(l_password, "arctangent", 10) != 0))
//    {
//        printf("ERROR: Wrong User-Password attribute value\n");
//        return RC_FAIL;
//    }

    // everything looks OK
    return RC_SUCCESS;
}


int ThreadRadiusAuthServer::sendResponse(RadiusServerStack &p_stack)
{
    // ----------------------
    // build response packet
    // ----------------------
    RadiusPacket l_response(D_PACKET_ACCESS_ACCEPT, p_stack.getRequest());
    RadiusAttribute l_attr;

    // add Service-Type attribute
    l_response.addAttribute(D_ATTR_SERVICE_TYPE, l_attr);
    l_attr.setNumber(1);

    // add Login-Service attribute
    l_response.addAttribute(D_ATTR_LOGIN_SERVICE, l_attr);
    l_attr.setNumber(0);

    // add Login-IP-Host attribute
    l_response.addAttribute(D_ATTR_LOGIN_IP_HOST, l_attr);
    struct in_addr l_addr;
    inet_aton("192.168.1.3", &l_addr);
    l_attr.setIPAddress(l_addr);

    // ----------------------
    // send response packet
    // ----------------------
    p_stack.sendResponse(l_response);

    // it's important to dump response packet _after_ calling sendResponse() method,
    // since the latter modifies response autheticator
    printf("---\nSend response packet\n---\n");
    l_response.dump();

    return RC_SUCCESS;
}

ThreadRadiusAuthServer::ThreadRadiusAuthServer() {
    id = idName();
    name = "Udp Server";
}
