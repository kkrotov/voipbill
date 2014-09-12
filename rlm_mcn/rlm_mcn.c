#include <freeradius-devel/ident.h>

RCSID("$Id$")

#include <freeradius-devel/radiusd.h>
#include <freeradius-devel/modules.h>

#include <fcntl.h>
#include <string.h>
#include <stdbool.h>
#include <sys/socket.h>

#define STATE_PARSE_REPLY_CODE 1
#define STATE_PARSE_KEY 2
#define STATE_PARSE_VALUE 3

typedef struct rlm_mcn_t {
    char *billing_host;
    int billing_port;
    char *message;
    int timeout_ms;

} MCN_INST;

static const CONF_PARSER module_config[] = {
    { "billing_host", PW_TYPE_STRING_PTR, offsetof(MCN_INST, billing_host), NULL, NULL},
    { "billing_port", PW_TYPE_INTEGER, offsetof(MCN_INST, billing_port), NULL, "0"},
    { "timeout_ms", PW_TYPE_INTEGER, offsetof(MCN_INST, timeout_ms), NULL, "100"},
    { "message", PW_TYPE_STRING_PTR, offsetof(MCN_INST, message), NULL, NULL},
    
    { NULL, -1, 0, NULL, NULL} /* end the list */
};

int mcn_init_socket(MCN_INST *instance, int *sock, struct sockaddr_in *addr) {
    struct hostent *hst;

    addr->sin_family = AF_INET;
    addr->sin_port = htons(instance->billing_port);
    addr->sin_addr.s_addr = inet_addr(instance->billing_host);
    if (addr->sin_addr.s_addr == INADDR_NONE) {
        if ((hst = gethostbyname(instance->billing_host)) && hst->h_addrtype == AF_INET) {
            addr->sin_addr.s_addr = *(in_addr_t *) hst->h_addr_list[0];
        } else {
            return -1;
        }
    }

    if ((*sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        return -1;
    }

    fcntl(*sock, F_SETFL, O_NONBLOCK);

    return 0;
}

void mcn_process_auth_request(char *request, char *response, int response_size, MCN_INST *instance) {

    int sock;
    struct sockaddr_in addr;

    *response = 0;

    if (mcn_init_socket(instance, &sock, &addr) < 0) {
        radlog(L_ERR, "rlm_mcn: Can not create socket");
        return;
    }

    fd_set eventset;
    FD_ZERO(&eventset);
    FD_SET(sock, &eventset);

    struct timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = instance->timeout_ms * 1000;

    sendto(sock, request, strlen(request), 0, (struct sockaddr *) &addr, sizeof(addr));

    select(sock + 1, &eventset, NULL, NULL, &timeout);

    if (FD_ISSET(sock, &eventset)) {
        int bytes_read = recv(sock, response, response_size - 1, 0);
        if (bytes_read > 0) {
            *(response + bytes_read) = 0;
        }
    }

    close(sock);
}

static int mcn_instantiate(CONF_SECTION *conf, void **instance) {
    MCN_INST *data;

    /*
     *	Set up a storage area for instance data
     */

    data = rad_malloc(sizeof (*data));
    if (!data) {
        return -1;
    }
    memset(data, 0, sizeof (*data));

    /*
     *	If the configuration parameters can't be parsed, then
     *	fail.
     */
    if (cf_section_parse(conf, data, module_config) < 0) {
        free(data);
        return -1;
    }

    if (data->billing_host == NULL || data->billing_port == 0 || data->message == NULL) {
        radlog(L_ERR, "rlm_mcn: Invalid module configuration");
        return -1;
    }

    *instance = data;

    return 0;
}

static int mcn_authorize(MCN_INST *instance, REQUEST *request) {
    // ???
    if (pairfind(request->packet->vps, PW_STATE) != NULL) {
        RDEBUG("!!!Found reply to access challenge");
        return RLM_MODULE_FAIL;
    }


    char auth_request[1024];
    char auth_response[1024];

    if (radius_xlat(auth_request, sizeof (auth_request), instance->message, request, NULL) < 0) {
        radlog(L_ERR, "rlm_mcn: Can not fill request. Check module configuration 'message' parameter");
        return RLM_MODULE_FAIL;
    }

    RDEBUG2("Request: %s", auth_request);

    mcn_process_auth_request(auth_request, auth_response, sizeof (auth_response), instance);

    RDEBUG2("Response2: %s", auth_response);


    char keyBuffer[256];
    char * pos = &auth_response[0];
    char * posStart = pos;
    int state = STATE_PARSE_REPLY_CODE;
    bool endOfLine = 0;
    while (!endOfLine) {
        endOfLine = *pos == 0;
        switch (state) {
            case STATE_PARSE_REPLY_CODE:
                if (*pos == ';' || endOfLine) {
                    *pos = 0;
                    if (strcmp(posStart, "1") == 0 || endOfLine) {
                        request->reply->code = PW_AUTHENTICATION_ACK;
                        RDEBUG2("Set reply code: PW_AUTHENTICATION_ACK");
                    } else {
                        request->reply->code = PW_AUTHENTICATION_REJECT;
                        RDEBUG2("Set reply code: PW_AUTHENTICATION_REJECT");
                    }
                    state = STATE_PARSE_KEY;
                    posStart = pos + 1;
                }
                break;
            case STATE_PARSE_KEY:
                if (*pos == '=') {
                    *pos = 0;
                    strncpy(keyBuffer, posStart, sizeof (keyBuffer));
                    state = STATE_PARSE_VALUE;
                    posStart = pos + 1;
                } else if (*pos == ';' || endOfLine) {
                    *pos = 0;
                    posStart = pos + 1;
                }
                break;
            case STATE_PARSE_VALUE:
                if (*pos == ';' || endOfLine) {
                    *pos = 0;

                    pairadd(
                            &request->reply->vps,
                            pairmake(keyBuffer, posStart, T_OP_EQ)
                            );

                    RDEBUG2("Set reply vps: %s = %s", keyBuffer, posStart);

                    state = STATE_PARSE_KEY;
                    posStart = pos + 1;
                }
                break;
        }

        pos += 1;

        if (endOfLine)
            break;
    }

    return RLM_MODULE_HANDLED;
}

static int mcn_detach(void *instance) {
    free(instance);
    return 0;
}


module_t rlm_mcn = {
    RLM_MODULE_INIT,
    "mcn",
    RLM_TYPE_THREAD_SAFE, /* type */
    mcn_instantiate, /* instantiation */
    mcn_detach, /* detach */
    {
        NULL, /* authentication */
        mcn_authorize, /* authorization */
        NULL, /* preaccounting */
        NULL, /* accounting */
        NULL, /* checksimul */
        NULL, /* pre-proxy */
        NULL, /* post-proxy */
        NULL /* post-auth */
    },
};
