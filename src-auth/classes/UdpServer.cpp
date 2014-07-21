#include "UdpServer.h"
#include "UdpMessageProcessor.h"

#include <ctime>
#include "../../src/classes/Log.h"

string make_daytime_string() {
    time_t now = time(0);
    return ctime(&now);
}

void UdpServer::handle_receive(const boost::system::error_code& error, size_t bytes_transferred) {
    if (!error || error == boost::asio::error::message_size) {

        recv_buffer_[bytes_transferred] = 0;

        string response = "GO_DEFAULT_ROUTE";

        try {

            auto processor = new UdpMessageProcessor(recv_buffer_.data());
            string response = processor->process();


        } catch (Exception &e) {
            e.addTrace("UdpServer ");
            Log::exception(e);
        } catch (std::exception &e) {
            Log::error("UdpServer: " + string(e.what()));
        } catch (...) {
            Log::error("UdpServer: ERROR");
        }

        boost::shared_ptr<string> message(new string(response));

        socket_.async_send_to(boost::asio::buffer(*message), remote_endpoint_,
                boost::bind(&UdpServer::handle_send, this, message,
                boost::asio::placeholders::error,
                boost::asio::placeholders::bytes_transferred));

        start_receive();
    }
}


