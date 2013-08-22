#include <map>

#include "Logger.h"

Logger::Logger() {
    grouping_period = 0;
}

Logger::~Logger() {
}

void Logger::logMessage(pLogMessage message) {
    lock_guard<std::mutex> lock(mutex);

    processGroupingMessages();

    pLogMessage history_message = history[message->message];
    if (history_message.get() == 0) {
        message->time = time(NULL);
        message->count = 1;
        history[message->message] = message;

        queue.push(message);
    } else {
        history_message->count++;
    }
}

void Logger::processGroupingMessages() {

    time_t period = getPeriod();
    if (period > grouping_period) {

        typename map<string, pLogMessage>::iterator i;
        typename map<string, pLogMessage>::iterator e;
        for (i = history.begin(), e = history.end(); i != e;) {
            pLogMessage message = i->second;
            if (message->count > 1) {
                message->time = grouping_period;
                queue.push(message);
            }
            i++;
        }

        history.clear();
        grouping_period = period;
    }
}

time_t Logger::getPeriod() {
    int group_interval_seconds = 60;

    time_t t = time(NULL);
    return t - t % group_interval_seconds + group_interval_seconds;
}

list<pLogMessage> Logger::getHistory() {
    lock_guard<std::mutex> lock(mutex);

    list<pLogMessage> res;
    typename map<string, pLogMessage>::iterator i;
    typename map<string, pLogMessage>::iterator e;
    for (i = history.begin(), e = history.end(); i != e;) {
        res.push_back(i->second);
        i++;
    }

    return res;
}

int Logger::getQueueLength() {
    lock_guard<std::mutex> lock(mutex);
    return queue.size();
}

bool Logger::isQueueEmpty() {
    lock_guard<std::mutex> lock(mutex);
    return queue.empty();
}

pLogMessage Logger::getQueueFront() {
    lock_guard<std::mutex> lock(mutex);
    return queue.front();
}

void Logger::queuePop() {
    lock_guard<std::mutex> lock(mutex);
    return queue.pop();
}