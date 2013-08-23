#include "Log.h"
#include <stdio.h>
#include "App.h"

#include "Loader.h"

void Log::info(const string &text) {
    Log::info(text.c_str());
}

void Log::error(const string &text) {
    Log::error(text.c_str());
}

void Log::info(const char * text) {
    pLogMessage message(new LogMessage);
    message->level = LogLevel::INFO;
    message->message = string(text);
    app.logger.logMessage(message);
}

void Log::error(const char * text) {
    pLogMessage message(new LogMessage);
    message->level = LogLevel::ERROR;
    message->message = string(text);
    app.logger.logMessage(message);
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