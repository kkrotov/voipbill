#pragma once

#include "BlackList.h"
#include "UdpControlClient.h"

class BlackListFull : public BlackList
{
public:

    static BlackListFull * instance(){
        static BlackListFull inst;
        return &inst;
    }

protected:

    bool udp_blacklist(vector<string> &list)
    {
        return UdpControlClient::blacklist(list);
    }

    bool udp_lock(string &phones)
    {
        return UdpControlClient::lock(phones);
    }

    bool udp_unlock(string &phones)
    {
        return UdpControlClient::unlock(phones);
    }

};
