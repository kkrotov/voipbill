#include "LogWriterSyslog.h"
#include <syslog.h>

LogWriterSyslog::LogWriterSyslog(string ident, LogLevel minLevel, LogLevel maxLevel) : BaseLogWriter(minLevel, maxLevel) {
    this->ident = ident;
}

bool LogWriterSyslog::beforePublish() {

    openlog(ident.c_str(), LOG_PID, LOG_LOCAL0);

}

void LogWriterSyslog::doPublish(pLogMessage message) {

    string msg = scount(message->count) + message->message;
    syslog(getSyslogLevel(message->level), "%s\n", msg.c_str());

}

void LogWriterSyslog::afterPublish() {

    closelog();

}

int LogWriterSyslog::getSyslogLevel(LogLevel level) {
    switch (level) {
        case LogLevel::DEBUG:
            return LOG_DEBUG;
        case LogLevel::INFO:
            return LOG_INFO;
        case LogLevel::NOTICE:
            return LOG_NOTICE;
        case LogLevel::WARNING:
            return LOG_WARNING;
        case LogLevel::ERROR:
            return LOG_ERR;
        case LogLevel::CRITICAL:
            return LOG_CRIT;
        default: return 0;
    }
}
