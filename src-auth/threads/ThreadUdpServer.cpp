#include "ThreadUdpServer.h"
#include "../classes/AppAuth.h"
#include "../classes/UdpServer.h"

void ThreadUdpServer::run() {

    try {
        boost::asio::io_service io_service;
        UdpServer server(io_service);
        io_service.run();
    } catch (exception& e) {
        Log::error("Error in ThreadUdpServer::run: " + string(e.what()));
        throw e;
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
