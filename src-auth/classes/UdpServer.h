#pragma once

#include <iostream>
#include <string>
#include <boost/array.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/asio.hpp>

using namespace std;

using boost::asio::ip::udp;
using boost::asio::ip::udp;

class UdpServer {
public:

    UdpServer(boost::asio::io_service& io_service)
    : socket_(io_service, udp::endpoint(udp::v4(), 8013)) {
        start_receive();
    }

private:

    void start_receive() {
        socket_.async_receive_from(
                boost::asio::buffer(recv_buffer_), remote_endpoint_,
                boost::bind(&UdpServer::handle_receive, this,
                boost::asio::placeholders::error,
                boost::asio::placeholders::bytes_transferred));
    }

    void handle_receive(const boost::system::error_code& error, size_t bytes_transferred);

    void handle_send(boost::shared_ptr<string> /*message*/,
            const boost::system::error_code& /*error*/,
            size_t /*bytes_transferred*/) {
    }

    udp::socket socket_;
    udp::endpoint remote_endpoint_;
    boost::array<char, 1024> recv_buffer_;
};

