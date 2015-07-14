#include "LogWriterScreen.h"

LogWriterScreen::LogWriterScreen(LogLevel minLevel, LogLevel maxLevel) : BaseLogWriter(minLevel, maxLevel) {
}

void LogWriterScreen::doPublish(pLogMessage message) {

    string msg = stime(message->time) + slevel(message->level) + scount(message->count) + message->message;
    printf("%s\n", msg.c_str());

}
