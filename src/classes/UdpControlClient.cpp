#include "UdpControlClient.h"

#include "App.h"
#include "Log.h"

bool UdpControlClient::sendrecv(string &msg, string &res){
	boost::asio::io_service io_service;
	udp::socket s(io_service);
	
    try {
		#ifdef _WIN32
			int32_t timeout = 1000;
			setsockopt(s.native(), SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof(timeout));
			setsockopt(s.native(), SOL_SOCKET, SO_SNDTIMEO, (const char*)&timeout, sizeof(timeout));
		#else
			struct timeval tv;
			tv.tv_sec  = 1;
			tv.tv_usec = 0;
			setsockopt(s.native(), SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
			setsockopt(s.native(), SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv));
		#endif

        boost::asio::ip::address addr = boost::asio::ip::address::from_string(app.conf.udp_host);
        udp::endpoint endpoint(addr, app.conf.udp_port);
        s.open(udp::v4());
		
        // send
        if (msg.size() != s.send_to(boost::asio::buffer(msg.data(), msg.size()), endpoint)) {
			s.close();
			return false;
		}

        // receive
        char recvdata[1024*20];
		size_t nread = s.receive(boost::asio::buffer(recvdata, sizeof(recvdata)));
		if (nread > 0)
			res.append(&recvdata[0], nread);

		s.close();
        return true;
    }
    catch (exception& e)
    {
        Log::error(e.what());
		s.close();
		return false;
    }
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
