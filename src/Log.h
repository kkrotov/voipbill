#pragma once

#include <boost/interprocess/sync/interprocess_mutex.hpp>
using namespace boost::interprocess;

#include <string>
using namespace std;

class Log {
protected:
    static interprocess_mutex mutex;

public:
    static void pr(const string &text);
    static void wr(const string &text);
    static void er(const string &text);

    static void pr(const char * text);
    static void wr(const char * text);
    static void er(const char * text);
};

