#include "UdpControlClient.h"

#include "AppBill.h"
#include "Log.h"

void UdpControlClient::handle_receive(const boost::system::error_code& error, size_t bytes_recvd, size_t *nread) {
    if (error) {
        Log::error(error.message());
        return;
    }
    if (nread != 0)
        *nread = bytes_recvd;
}

bool UdpControlClient::sendrecv(string &msg, string &res) {
    boost::asio::io_service io_service;
    udp::socket s(io_service);

    string debug_msg = "OpenCA: ";

    try {
        boost::asio::ip::address addr = boost::asio::ip::address::from_string(app().conf.openca_udp_host);
        udp::endpoint endpoint(addr, app().conf.openca_udp_port);

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
        timeout.tv_sec = 2;
        timeout.tv_usec = 0;

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

bool UdpControlClient::ready() {
    return app().conf.openca_udp_host != "" && app().conf.openca_udp_port != 0;
}

bool UdpControlClient::select_calls(vector<string> &list) {
    string msg("SELECT");
    string res;
    if (!sendrecv(msg, res)) return false;
    if (res == "") {
        list.empty();
        return true;
    }
    boost::algorithm::split(list, res, boost::algorithm::is_any_of(","), boost::token_compress_on);
    return true;
}

bool UdpControlClient::kill(string &phones, string &ids) {
    string msg("KILL " + phones + " " + ids);
    string res;
    return sendrecv(msg, res);
}

bool UdpControlClient::blacklist_local(vector<string> &list) {
    string msg("READ_BLACKLIST_LOCAL");
    string res;
    if (!sendrecv(msg, res) || res == "0") return false;
    if (res == "") {
        list.empty();
        return true;
    }
    boost::algorithm::split(list, res, boost::algorithm::is_any_of(","), boost::token_compress_on);
    return true;
}

bool UdpControlClient::blacklist_global(vector<string> &list) {
    string msg("READ_BLACKLIST_GLOBAL");
    string res;
    if (!sendrecv(msg, res) || res == "0") return false;
    if (res == "") {
        list.empty();
        return true;
    }
    boost::algorithm::split(list, res, boost::algorithm::is_any_of(","), boost::token_compress_on);
    return true;
}

bool UdpControlClient::blacklist_trunk(vector<string> &list) {
    string msg("READ_LOCKED_TRUNK");
    string res;
    if (!sendrecv(msg, res) || res == "0") return false;
    if (res == "") {
        list.empty();
        return true;
    }
    boost::algorithm::split(list, res, boost::algorithm::is_any_of(","), boost::token_compress_on);
    return true;
}

bool UdpControlClient::lock_local(const string &phone) {
    string msg("LOCK_LOCAL " + phone);
    string res;
    return sendrecv(msg, res) && res == "1";
}

bool UdpControlClient::lock_global(const string &phone) {
    string msg("LOCK_GLOBAL " + phone);
    string res;
    return sendrecv(msg, res) && res == "1";
}

bool UdpControlClient::lock_trunk(const string &trunk) {
    string msg("LOCK_TRUNK " + trunk);
    string res;
    return sendrecv(msg, res) && res == "1";
}

bool UdpControlClient::unlock_local(const string &phone) {
    string msg("UNLOCK_LOCAL " + phone);
    string res;
    return sendrecv(msg, res) && res == "1";
}

bool UdpControlClient::unlock_global(const string &phone) {
    string msg("UNLOCK_GLOBAL " + phone);
    string res;
    return sendrecv(msg, res) && res == "1";
}

bool UdpControlClient::unlock_trunk(const string &trunk) {
    string msg("UNLOCK_TRUNK " + trunk);
    string res;
    return sendrecv(msg, res) && res == "1";
}
