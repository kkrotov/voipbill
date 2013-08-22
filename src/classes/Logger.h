#pragma once

#include <string>
#include <queue>
#include <map>
#include <list>
#include <memory>
#include <mutex>
using namespace std;

enum LogLevel {
    DEBUG, INFO, NOTICE, WARNING, ERROR, CRITICAL
};

typedef struct _LogMessage {
    LogLevel level;
    time_t time;
    string message;
    string extended_message;
    int count;
} LogMessage;

typedef shared_ptr<LogMessage> pLogMessage;

class Logger {
protected:
    std::queue<pLogMessage> queue;
    std::map<string, pLogMessage> history;
    std::mutex mutex;
public:
    Logger();
    Logger(const Logger& orig);
    virtual ~Logger();

    void logMessage(pLogMessage message);
    list<pLogMessage> getHistory();
    int getQueueLength();
    bool isQueueEmpty();
    pLogMessage getQueueFront();
    void queuePop();
private:
    time_t grouping_period;
    time_t getPeriod();
    void addMessageToHistory();
    void processGroupingMessages();
};
