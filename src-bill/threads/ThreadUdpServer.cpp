#include "ThreadUdpServer.h"
#include "../classes/AppBill.h"
#include "../classes/UdpMessageProcessor.h"

#include <boost/array.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/asio.hpp>


using boost::asio::ip::udp;

void ThreadUdpServer::reopenSocket() {
    if (!socket.is_open() && status != ThreadStatus::THREAD_STOPPED) {
        udp::socket socket{io_service, udp::endpoint(udp::v4(), app().conf.api_port)};
        std::swap(this->socket, socket);
    }
}

void ThreadUdpServer::onShutdown() {
    if (socket.is_open()) {
        shutdown(socket.native_handle(), SHUT_RDWR);
    }
}

void ThreadUdpServer::threadCleanup(Thread* thisThread) {
    if (socket.is_open()) {
        socket.close();
    }
}

void ThreadUdpServer::receive(boost::array<char, 1024>& recv_buf, udp::endpoint& remote_endpoint) {
    boost::system::error_code error;

    int bytes_received = socket.receive_from(boost::asio::buffer(recv_buf), remote_endpoint, 0, error);

    if (error && error != boost::asio::error::message_size)
        throw boost::system::system_error(error);

    recv_buf[bytes_received] = 0;
}

string ThreadUdpServer::process(const string &message) {
    auto processor = new UdpMessageProcessor(message, &db_calls);
    return processor->process();
}

void ThreadUdpServer::send(const string &message, const udp::endpoint& remote_endpoint) {
    boost::system::error_code ignored_error;
    socket.send_to(boost::asio::buffer(message),
            remote_endpoint, 0, ignored_error);
}

void ThreadUdpServer::run() {
    try {
        reopenSocket();

        while (status != ThreadStatus::THREAD_STOPPED) {
            boost::array<char, 1024> recv_buf;
            udp::endpoint remote_endpoint;
            
            receive(recv_buf, remote_endpoint);
            string response = process(recv_buf.data());

            send(response, remote_endpoint);
        }
    } catch (exception& e) {
        Log::error("Error in ThreadUdpServer::run: " + string(e.what()));
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

ThreadUdpServer::ThreadUdpServer() :
    socket{io_service} {
    id = idName();
    name = "Udp Server";

    db_calls.setCS(app().conf.db_calls);
    
    errors = 0;
    
    onFinished.connect(boost::bind(&ThreadUdpServer::threadCleanup, this, _1));
}
