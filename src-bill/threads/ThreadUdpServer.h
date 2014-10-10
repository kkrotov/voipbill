#pragma once

#include "../../src/threads/Thread.h"
#include "../../src/classes/BDb.h"
#include "../../src/classes/Timer.h"
#include <boost/asio.hpp>

using boost::asio::ip::udp;

class ThreadUdpServer : public Thread {
    int errors;
    BDb db_calls;
    
    boost::asio::io_service io_service;
    udp::socket socket;

    void run();

    void htmlfull(stringstream &html);
    
    void reopenSocket();
    
    virtual void onShutdown() override;
    
    void threadCleanup(Thread* thisThread);
    
    void receive(boost::array<char, 1024>& recv_buf, udp::endpoint& remote_endpoint);
    string process(const string &message);
    void send(const string &message, const udp::endpoint& remote_endpoint);

public:

    ThreadUdpServer();
    static const char* idName() { return "udp_server"; }
};

