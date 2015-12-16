#pragma once

#include "BlackList.h"
#include "UdpControlClient.h"

class BlackListAntiFraudDisable : public BlackList {
public:

    static BlackListAntiFraudDisable * instance() {
        static BlackListAntiFraudDisable inst;
        return &inst;
    }

protected:

    bool udp_blacklist(vector<string> &list) {
        return UdpControlClient::blacklist_anti_fraud_disable(list);
    }

    bool udp_lock(const string &phone) {
        return UdpControlClient::add_anti_fraud_disable(phone);
    }

    bool udp_unlock(const string &phone) {
        return UdpControlClient::del_anti_fraud_disable(phone);
    }

};
