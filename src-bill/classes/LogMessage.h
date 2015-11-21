#pragma once

#include <string>
#include <memory>
#include <map>

enum LogLevel {
    DEBUG, INFO, NOTICE, WARNING, ERROR, CRITICAL
};

struct LogMessage {
    LogLevel level;
    std::string message;
    std::string type;
    time_t time;
    time_t timeInGroup;
    int count;
    int countInGroup;
    std::map<std::string, std::string> params;
};

typedef std::shared_ptr<LogMessage> pLogMessage;