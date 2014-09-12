#pragma once

#include "../../src/common.h"

#include <boost/asio.hpp>
using boost::asio::ip::udp;

class UdpControlClient {
protected:

    static void handle_receive(const boost::system::error_code& error, size_t bytes_recvd, size_t *nread);

    static bool sendrecv(string &msg, string &res);

public:
    static bool ready();

    static bool select_calls(vector<string> &list);

    static bool kill(string &phones, string &ids);

    static bool blacklist(vector<string> &list);

    static bool blacklist_local(vector<string> &list);

    static bool blacklist_global(vector<string> &list);

    static bool blacklist_trunk(vector<string> &list);

    static bool lock(string &phone);

    static bool lock_local(string &phone);

    static bool lock_global(string &phone);

    static bool lock_trunk(string &phone);

    static bool unlock(string &trunk);

    static bool unlock_local(string &phone);

    static bool unlock_global(string &phone);

    static bool unlock_trunk(string &trunk);
};
