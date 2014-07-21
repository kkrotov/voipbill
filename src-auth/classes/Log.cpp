#include "../../src/classes/Log.h"
#include <stdio.h>
#include "AppAuth.h"

void Log::info(const string &text) {
    pLogMessage message(new LogMessage);
    message->level = LogLevel::INFO;
    message->message = text;
    app().logger.logMessage(message);
}

void Log::error(const string &text) {
    pLogMessage message(new LogMessage);
    message->level = LogLevel::ERROR;
    message->message = text;
    app().logger.logMessage(message);
}

void Log::notice(const string &text) {
    pLogMessage message(new LogMessage);
    message->level = LogLevel::NOTICE;
    message->message = text;
    app().logger.logMessage(message);
}

void Log::warning(const string &text) {
    pLogMessage message(new LogMessage);
    message->level = LogLevel::WARNING;
    message->message = text;
    app().logger.logMessage(message);
}

void Log::debug(const string &text) {
    pLogMessage message(new LogMessage);
    message->level = LogLevel::DEBUG;
    message->message = text;
    app().logger.logMessage(message);
}

void Log::exception(Exception &e) {

    string message = "";

    for (auto it = e.traceList.begin(); it != e.traceList.end(); ++it) {
        message += *it + " -> ";
    }

    if (message.size() > 0) {
        message += "\n";
    }

    message += e.what();

    Log::error(message);
}