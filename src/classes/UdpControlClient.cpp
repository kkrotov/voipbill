#include "UdpControlClient.h"

#include "App.h"
#include "Log.h"

void UdpControlClient::handle_receive(const boost::system::error_code& error, size_t bytes_recvd, size_t *nread)
{
    if (error)
    {
        Log::er(error.message());
        return;
    }
    if (nread != 0)
        *nread = bytes_recvd;
}

bool UdpControlClient::sendrecv(string &msg, string &res){
    try {
        boost::asio::ip::address addr = boost::asio::ip::address::from_string(app.conf.udp_host);

        udp::endpoint endpoint(addr, app.conf.udp_port);

        boost::asio::io_service io_service;
        udp::socket s(io_service);
        s.open(udp::v4());

        // send
        if (msg.size() != s.send_to(boost::asio::buffer(msg.data(), msg.size()), endpoint)) return false;

        // receive
        size_t nread = 0xFFFF;
        char recvdata[1024*20];

        boost::asio::deadline_timer timer(io_service);
        timer.expires_from_now(boost::posix_time::seconds(3));
        timer.async_wait(boost::bind(UdpControlClient::handle_receive,
                                     boost::asio::placeholders::error, 0, (size_t *)0));


        s.async_receive(boost::asio::buffer(recvdata, sizeof(recvdata)),
                        boost::bind(UdpControlClient::handle_receive,
                                  boost::asio::placeholders::error,
                                  boost::asio::placeholders::bytes_transferred, &nread));

        io_service.reset();
        io_service.run_one();
        timer.cancel();
        s.cancel();

        if (nread == 0xFFFF) return false;

        if (nread > 0)
            res.append(&recvdata[0], nread);

        return true;
    }
    catch (exception& e)
    {
        Log::er(e.what());
    }
    return false;
}

bool UdpControlClient::ready(){
    return app.conf.udp_host != "" && app.conf.udp_port != 0;
}

bool UdpControlClient::select(vector<string> &list){
    string msg("SELECT");
    string res;
    if (sendrecv(msg, res) == false) return false;

    boost::algorithm::split(list, res, boost::algorithm::is_any_of(","));
    return true;
}

bool UdpControlClient::kill(string &phones, string &ids){
    string msg("KILL " + phones + " " + ids);
    string res;
    return sendrecv(msg, res);
}

bool UdpControlClient::blacklist_local(vector<string> &list){
    string msg("READ_BLACKLIST_LOCAL");
    string res;
    if (sendrecv(msg, res) == false || res == "0") return false;
    boost::algorithm::split(list, res, boost::algorithm::is_any_of(","));
    return true;
}

bool UdpControlClient::blacklist_global(vector<string> &list){
    string msg("READ_BLACKLIST_GLOBAL");
    string res;
    if (sendrecv(msg, res) == false || res == "0") return false;
    boost::algorithm::split(list, res, boost::algorithm::is_any_of(","));
    return true;
}

bool UdpControlClient::lock_local(string &phones){
    string msg("LOCK_LOCAL " + phones);
    string res;
    return sendrecv(msg, res) && res == "1";
}

bool UdpControlClient::lock_global(string &phones){
    string msg("LOCK_GLOBAL " + phones);
    string res;
    return sendrecv(msg, res) && res == "1";
}

bool UdpControlClient::unlock_local(string &phones){
    string msg("UNLOCK_LOCAL " + phones);
    string res;
    return sendrecv(msg, res) && res == "1";
}

bool UdpControlClient::unlock_global(string &phones){
    string msg("UNLOCK_GLOBAL " + phones);
    string res;
    return sendrecv(msg, res) && res == "1";
}
