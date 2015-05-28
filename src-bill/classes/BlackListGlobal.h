#pragma once

#include "BlackList.h"
#include "UdpControlClient.h"

class BlackListGlobal : public BlackList {
public:

    static BlackListGlobal * instance() {
        static BlackListGlobal inst;
        return &inst;
    }

protected:

    bool udp_blacklist(vector<string> &list) {
        return UdpControlClient::blacklist_global(list);
    }

    bool udp_lock(const string &phone) {
        return UdpControlClient::lock_global(phone);
    }

    bool udp_unlock(const string &phone) {
        return UdpControlClient::unlock_global(phone);
    }

};
