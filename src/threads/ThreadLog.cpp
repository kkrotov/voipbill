#include "ThreadLog.h"

void ThreadLog::run() {


    while (true) {
        ssleep(1);

        while (!app.logger.isQueueEmpty()) {
            pLogMessage message = app.logger.getQueueFront();

            struct tm * timeinfo;
            char stime [30];
            char * pstime = &stime[0];
            timeinfo = localtime(&message->time);
            strftime(stime, 30, "[%d.%m.%Y %H:%M:%S]", timeinfo);

            string msg = message->message;
            if (message->count > 1) {
                msg = "[" + lexical_cast<string>(message->count) + "] " + msg;
            }

            printf("%s %s\n", pstime, msg.c_str());

            if (message->level < LogLevel::ERROR) {
                FILE* f = fopen(app.conf.log_file.c_str(), "a");
                if (f != 0) {
                    fprintf(f, "%s %s\n", pstime, msg.c_str());
                    fclose(f);
                }
            }

            if (message->level >= LogLevel::ERROR) {
                FILE* f2 = fopen(app.conf.err_log_file.c_str(), "a");
                if (f2 != 0) {
                    fprintf(f2, "%s %s\n", pstime, msg.c_str());
                    fclose(f2);
                }
            }

            app.logger.queuePop();
        }

    }
}

void ThreadLog::htmlfull(stringstream & html) {

    this->html(html);

    list<pLogMessage> history = app.logger.getHistory();

    html << "Queue length: <b>" << app.logger.getQueueLength() << "</b><br/>\n";
    html << "History length: <b>" << history.size() << "</b><br/>\n";
    html << "<br/>\n";

    for (auto it = history.begin(); it != history.end(); ++it) {
        pLogMessage message = *it;
        html << "<b>" << message->count << "</b> " << string_time(message->time) << " ";
        html << message->message.substr(0, 60);
        html << "<br/>\n";
    }

}

ThreadLog::ThreadLog() {
    id = "log";
    name = "Log";
}


