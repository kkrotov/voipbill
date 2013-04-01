#pragma once

#include "BlackList.h"
#include "UdpControlClient.h"

class BlackListGlobal : public BlackList
{
public:

    static BlackListGlobal * instance(){
        static BlackListGlobal inst;
        return &inst;
    }

protected:

    bool udp_blacklist(vector<string> &list)
    {
        return UdpControlClient::blacklist_global(list);
    }

    bool udp_lock(string &phones)
    {
        return UdpControlClient::lock_global(phones);
    }

    bool udp_unlock(string &phones)
    {
        return UdpControlClient::unlock_global(phones);
    }

};
