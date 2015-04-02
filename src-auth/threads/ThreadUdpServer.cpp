#include "ThreadUdpServer.h"
#include "../classes/AppAuth.h"
#include "../classes/UdpMessageProcessor.h"

#include <boost/array.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/asio.hpp>


using boost::asio::ip::udp;

void ThreadUdpServer::run() {

    boost::asio::io_service io_service;

    udp::socket socket(io_service, udp::endpoint(udp::v4(), app().conf.api_port));

    for (;;) {
        boost::array<char, 1024> recv_buf;
        udp::endpoint remote_endpoint;
        boost::system::error_code error;



        int bytes_received = socket.receive_from(boost::asio::buffer(recv_buf), remote_endpoint, 0, error);



        if (error && error != boost::asio::error::message_size)
            throw boost::system::system_error(error);



        recv_buf[bytes_received] = 0;



        auto processor = new UdpMessageProcessor(recv_buf.data());
        string response = processor->process();

        Log::debug("RESPONSE: " + response);


        boost::system::error_code ignored_error;
        socket.send_to(boost::asio::buffer(response),
                remote_endpoint, 0, ignored_error);
    }

}

void ThreadUdpServer::htmlfull(stringstream &html) {
    this->html(html);

    html << "Time loop: <b>" << t.sloop() + "</b><br/>\n";
    html << "Time full loop: <b>" << t.sfull() + "</b><br/>\n";
    html << "loops: <b>" << t.count << "</b><br/>\n";
    html << "<br/>\n";

    html << "Errors count: <b>" << errors << "</b><br/>\n";
}

ThreadUdpServer::ThreadUdpServer() {
    id = "udp_server";
    name = "Udp Server";

    errors = 0;
}