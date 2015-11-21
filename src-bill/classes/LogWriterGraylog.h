#pragma once

#include "BaseLogWriter.h"

class LogWriterGraylog : public BaseLogWriter {
public:
    LogWriterGraylog(string graylogServer, LogLevel minLevel = LogLevel::DEBUG, LogLevel maxLevel = LogLevel::CRITICAL);
protected:
    string graylogServer;
    bool beforePublish();
    void doPublish(pLogMessage message);
    void afterPublish();
    int getGraylogLevel(LogLevel level);
};


