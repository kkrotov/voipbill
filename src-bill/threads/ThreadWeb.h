#pragma once

#include <map>
#include <string>
#include <boost/asio.hpp>

using namespace std;

class ThreadWeb {
public:

    void operator()();

    void stop();

    static void handlerHeader(stringstream &html);

    static void handlerHome(stringstream &html);

    static void handlerConfig(stringstream &html);

    static bool handlerTask(stringstream &html, map<string, string> &parameters);

    static void handlerCounters(stringstream &html);

    static void handlerClient(stringstream &html, map<string, string> &parameters);

private:
    boost::asio::io_service io_service;
};
