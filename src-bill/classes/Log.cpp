#include "Log.h"
#include "AppBill.h"

void Log::info(const string &text) {
    pLogMessage message(new LogMessage);
    message->level = LogLevel::INFO;
    message->message = text;
    app().logger.logMessage(message);
}

void Log::info(pLogMessage message) {
    message->level = LogLevel::INFO;
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

void Log::notice(pLogMessage message) {
    message->level = LogLevel::NOTICE;
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

    Log::error(e.getFullMessage());

}

void Log::flush() {
    app().logger.processLogQueue();
}