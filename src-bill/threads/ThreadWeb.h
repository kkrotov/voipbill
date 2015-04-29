#pragma once

#include <map>
#include <string>
#include <boost/asio.hpp>
#include "../../src/http/reply.hpp"
#include "../../src/http/request.hpp"

using namespace std;

class ThreadWeb {
public:

    void operator()();

    void stop();

private:
    boost::asio::io_service io_service;
};
