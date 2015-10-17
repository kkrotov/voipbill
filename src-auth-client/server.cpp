/*
 * This source file is part of the ace-radius library.  This code was
 * written by Alex Agranov in 2004-2009, and is covered by the BSD open source
 * license. Refer to the accompanying documentation for details on usage and
 * license.
 */

#include <stdio.h>
#include <string.h>

#include "../libs/ace-radius/RadiusServerStack.h"

/*
 * A sample RADIUS server.
 *
 * Usage:
 *     server
 *
 * Notes:
 *   - server listens on authentication port (1812)
 *   - it answers only Access-Requests with user="nemo" and password="arctangent"
 */

// ----------------------
// verify request packet
// ----------------------
int verifyRequest(RadiusPacket & p_request)
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


int sendResponse(RadiusServerStack &p_stack)
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

int main (int argc, char *argv[])
{
    int l_rc;

    printf("Test RADIUS Server application\n");

    // create instance of server stack
    RadiusServerStack l_stack(RadiusSecret("427a7663dbd035c5f7611c9545e9d0"));
    if (!l_stack.isValid())
    {
        printf("Can not create RADIUS server stack\n");
        return 1;
    }

    // reference to request packet
    RadiusPacket & l_request = l_stack.getRequest();

    while (1)
    {
        l_rc = l_stack.receiveRequest();
        if (l_rc == RC_SUCCESS)
        {
            printf("---\nReceived request packet from ");
            l_stack.getClientAddress().dump();
            printf("\n---\n");
            l_request.dump();

            if (verifyRequest(l_stack.getRequest()) == RC_SUCCESS)
                sendResponse(l_stack);
        }
        else
            printf("Error 1\n");
    }

    return 0;
}
