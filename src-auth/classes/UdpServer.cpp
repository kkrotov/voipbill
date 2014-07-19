#include "UdpServer.h"
#include "UdpMessageProcessor.h"

#include <ctime>

string make_daytime_string() {
    time_t now = time(0);
    return ctime(&now);
}

void UdpServer::handle_receive(const boost::system::error_code& error, size_t bytes_transferred) {
    if (!error || error == boost::asio::error::message_size) {

        recv_buffer_[bytes_transferred] = 0;

        auto processor = new UdpMessageProcessor(recv_buffer_.data());

        boost::shared_ptr<string> message(new string(processor->process()));

        socket_.async_send_to(boost::asio::buffer(*message), remote_endpoint_,
                boost::bind(&UdpServer::handle_send, this, message,
                boost::asio::placeholders::error,
                boost::asio::placeholders::bytes_transferred));

        start_receive();
    }
}


