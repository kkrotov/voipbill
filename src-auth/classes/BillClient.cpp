#include "BillClient.h"

#include "AppAuth.h"
#include "../../src/classes/Log.h"

void BillClient::handle_receive(const boost::system::error_code& error, size_t bytes_recvd, size_t *nread) {
    if (error) {
        Log::error(error.message());
        return;
    }
    if (nread != 0)
        *nread = bytes_recvd;
}

bool BillClient::sendrecv(string &msg, string &res) {
    boost::asio::io_service io_service;
    udp::socket s(io_service);

    string debug_msg = "AuthClient: ";

    try {
        boost::asio::ip::address addr = boost::asio::ip::address::from_string(app().conf.auth_api_host);
        udp::endpoint endpoint(addr, app().conf.auth_api_port);

        s.open(udp::v4());
        s.connect(endpoint);

        debug_msg += msg + ": ";

        // send
        if (msg.size() != s.send(boost::asio::buffer(msg.data(), msg.size()))) {
            s.close();
            Log::error(debug_msg + "Send error");
            return false;
        }

        fcntl(s.native(), F_SETFL, O_NONBLOCK);

        fd_set readset;
        FD_ZERO(&readset);
        FD_SET(s.native(), &readset);

        timeval timeout;
        timeout.tv_sec = 0;
        timeout.tv_usec = 10000; // 10 milliseconds

        select(s.native() + 1, &readset, NULL, NULL, &timeout);

        if (FD_ISSET(s.native(), &readset)) {
            char recvdata[1024 * 20];
            int bytes_read = recv(s.native(), recvdata, sizeof (recvdata), 0);

            if (bytes_read > 0) {
                res.append(&recvdata[0], bytes_read);
                s.close();
            }

            if (bytes_read >= 0) {
                Log::debug(debug_msg + res);
                return true;
            }
        }

    } catch (exception& e) {
        Log::error(e.what());
    }

    s.close();
    Log::error(debug_msg + "Receive error");
    return false;
}

string BillClient::query(string callingStationId, string calledStationId) {

    string msg = "calling:" + callingStationId + ";called:" + calledStationId;
    string res;

    if (sendrecv(msg, res) == false) return "accept";

    return res;
}
