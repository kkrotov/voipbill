#pragma once

#include "../common.h"

#include <boost/asio.hpp>
using boost::asio::ip::udp;

class UdpControlClient {
protected:

    static bool sendrecv(string &msg, string &res);

public:
    static bool ready();

    static bool select(vector<string> &list);

    static bool kill(string &phones, string &ids);

    static bool blacklist(vector<string> &list);

    static bool blacklist_local(vector<string> &list);

    static bool blacklist_global(vector<string> &list);

    static bool lock(string &phones);

    static bool lock_local(string &phones);

    static bool lock_global(string &phones);

    static bool unlock(string &phones);

    static bool unlock_local(string &phones);

    static bool unlock_global(string &phones);

};
