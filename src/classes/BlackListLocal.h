#pragma once

#include "BlackList.h"
#include "UdpControlClient.h"

class BlackListLocal : public BlackList {
public:

    static BlackListLocal * instance() {
        static BlackListLocal inst;
        return &inst;
    }

protected:

    bool udp_blacklist(vector<string> &list) {
        return UdpControlClient::blacklist_local(list);
    }

    bool udp_lock(string &phones) {
        return UdpControlClient::lock_local(phones);
    }

    bool udp_unlock(string &phones) {
        return UdpControlClient::unlock_local(phones);
    }

};
