#include "ThreadLog.h"
#include "../classes/AppBill.h"

void ThreadLog::run() {

    app().logger.processLogQueue();

}

void ThreadLog::htmlfull(stringstream & html) {

    this->html(html);

    list<pLogMessage> history = app().logger.getHistory();

    html << "Queue length: <b>" << app().logger.getQueueLength() << "</b><br/>\n";
    html << "History length: <b>" << history.size() << "</b><br/>\n";
    html << "<br/>\n";

    for (auto it = history.begin(); it != history.end(); ++it) {
        pLogMessage message = *it;
        html << "<b>" << message->countInGroup << "</b> " << string_time(message->timeInGroup) << " ";
        html << message->message.substr(0, 60);
        html << "<br/>\n";
    }

}

ThreadLog::ThreadLog() {
    id = idName();
    name = "Log";
}


