#pragma once

#include "common.h"

#include <boost/asio.hpp>
using boost::asio::ip::udp;

class UdpControlClient {

protected:

    static void handle_receive(const boost::system::error_code& error, size_t bytes_recvd, size_t *nread);

    static bool sendrecv(string &msg, string &res);

public:
    static bool ready();

    static bool select(vector<string> &list);

    static bool blacklist(vector<string> &list);

    static bool kill(string &phones, string &ids);

    static bool lock(string &phones);

    static bool unlock(string &phones);
};
