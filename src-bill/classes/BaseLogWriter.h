#pragma once

#include <string>
#include <list>
#include <memory>
using namespace std;

enum LogLevel {
    DEBUG, INFO, NOTICE, WARNING, ERROR, CRITICAL
};

typedef struct _LogMessage {
    LogLevel level;
    string message;
    string extended_message;
    time_t time;
    time_t timeInGroup;
    int count;
    int countInGroup;
} LogMessage;

typedef shared_ptr<LogMessage> pLogMessage;

class BaseLogWriter {
public:
    BaseLogWriter(LogLevel minLevel, LogLevel maxLevel);
    void publish(pLogMessage message);
    void massPublish(list<pLogMessage> messages);
protected:
    LogLevel minLevel;
    LogLevel maxLevel;
    virtual bool beforePublish();
    virtual void doPublish(pLogMessage message) = 0;
    virtual void afterPublish();
    string stime(time_t time);
    string slevel(LogLevel level);
    string scount(int count);
};

typedef shared_ptr<BaseLogWriter> pLogWriter;