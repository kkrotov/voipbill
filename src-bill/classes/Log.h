#pragma once

#include "Exception.h"
#include "LogMessage.h"

using namespace std;

class Log {
public:
    static void info(const string &text);
    static void info(pLogMessage message);
    static void error(const string &text);
    static void notice(const string &text);
    static void warning(const string &text);
    static void debug(const string &text);

    static void exception(Exception &e);

    static void flush();
};

