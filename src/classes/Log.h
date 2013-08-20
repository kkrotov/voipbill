#pragma once

#include <mutex>
#include <string>

#include "Exception.h"
using namespace std;

class Log {
protected:
    static mutex log_mutex;

public:
    static void info(const string &text);
    static void error(const string &text);

    static void info(const char * text);
    static void error(const char * text);

    static void exception(Exception &e);
};

