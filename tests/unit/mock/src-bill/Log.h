#pragma once
#include <time.h>
#include <string.h>
class Log {
    static void error (const std::string& str, time_t t=0);
};
