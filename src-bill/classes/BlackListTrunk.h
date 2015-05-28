#pragma once

#include "BlackList.h"
#include "UdpControlClient.h"

class BlackListTrunk : public BlackList {
public:

    static BlackListTrunk * instance() {
        static BlackListTrunk inst;
        return &inst;
    }

protected:

    bool udp_blacklist(vector<string> &list) {
        return UdpControlClient::blacklist_trunk(list);
    }

    bool udp_lock(const string &trunk) {
        return UdpControlClient::lock_trunk(trunk);
    }

    bool udp_unlock(const string &trunk) {
        return UdpControlClient::unlock_trunk(trunk);
    }

};
