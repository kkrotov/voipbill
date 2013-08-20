#include "Log.h"
#include <stdio.h>
#include "App.h"

mutex Log::log_mutex;

void Log::info(const string &text) {
    Log::info(text.c_str());
}

void Log::error(const string &text) {
    Log::error(text.c_str());
}

void Log::info(const char * text) {
    time_t rawtime;
    struct tm * timeinfo;
    char stime [30];
    char * pstime = &stime[0];
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    strftime(stime, 30, "[%d.%m.%Y %H:%M:%S]", timeinfo);

    log_mutex.lock();

    printf("%s %s\n", pstime, text);

    FILE* f = fopen(app.conf.log_file.c_str(), "a");
    if (f != 0) {
        fprintf(f, "%s %s\n", pstime, text);
        fclose(f);
    }

    log_mutex.unlock();
}

void Log::error(const char * text) {
    time_t rawtime;
    struct tm * timeinfo;
    char stime [30];
    char * pstime = &stime[0];
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    strftime(stime, 30, "[%d.%m.%Y %H:%M:%S]", timeinfo);

    log_mutex.lock();

    printf("%s %s\n", pstime, text);

    FILE* f = fopen(app.conf.log_file.c_str(), "a");
    if (f != 0) {
        fprintf(f, "%s %s\n", pstime, text);
        fclose(f);
    }

    FILE* f2 = fopen(app.conf.err_log_file.c_str(), "a");
    if (f2 != 0) {
        fprintf(f2, "%s %s\n", pstime, text);
        fclose(f2);
    }

    log_mutex.unlock();
}

void Log::exception(Exception &e) {
    string message = "";
    for (auto it = e.traceList.begin(); it != e.traceList.end(); ++it) {
        message += *it + "\n";
    }
    message += e.what();

    Log::error(message);
}