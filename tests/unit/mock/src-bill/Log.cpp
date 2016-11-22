#include <stdio.h>
#include <string.h>
#include <time.h>
#include <iostream>
#include "Log.h"
void Log::error(const std::string &text, time_t log_time) {

    std::cout << text << '\n';
}
