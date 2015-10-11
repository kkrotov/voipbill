#pragma once

#include "../common.h"

#include <boost/asio.hpp>
using boost::asio::ip::udp;

class BillClient {
protected:

    static void handle_receive(const boost::system::error_code& error, size_t bytes_recvd, size_t *nread);

    static bool sendrecv(string &msg, string &res);

public:
    
    static string query(string aNumber, string bNumber, string trunkName);

};
