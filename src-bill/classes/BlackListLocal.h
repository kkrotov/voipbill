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

    bool udp_lock(string &phone) {
        return UdpControlClient::lock_local(phone);
    }

    bool udp_unlock(string &phone) {
        return UdpControlClient::unlock_local(phone);
    }

};
