#include "LogWriterGraylog.h"

LogWriterGraylog::LogWriterGraylog(string graylogServer, LogLevel minLevel, LogLevel maxLevel) : BaseLogWriter(minLevel, maxLevel) {
    this->graylogServer = graylogServer;
}

bool LogWriterGraylog::beforePublish() {

    return true;

}

void LogWriterGraylog::doPublish(pLogMessage message) {


}

void LogWriterGraylog::afterPublish() {


}

int LogWriterGraylog::getGraylogLevel(LogLevel level) {
    switch (level) {
        case LogLevel::DEBUG:
            return 7;
        case LogLevel::INFO:
            return 6;
        case LogLevel::NOTICE:
            return 5;
        case LogLevel::WARNING:
            return 4;
        case LogLevel::ERROR:
            return 3;
        case LogLevel::CRITICAL:
            return 2;
        default: return 0;
    }
}
