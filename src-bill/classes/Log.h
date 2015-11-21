#pragma once

#include <string>

#include "Exception.h"
using namespace std;

class Log {
public:
    static void info(const string &text);
    static void error(const string &text);
    static void notice(const string &text);
    static void warning(const string &text);
    static void debug(const string &text);

    static void exception(Exception &e);

    static void flush();
};

