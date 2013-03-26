#include "Log.h"
#include <stdio.h>
#include "App.h"

interprocess_mutex Log::mutex;

void Log::pr(const string &text){
    Log::pr(text.c_str());
}
void Log::wr(const string &text){
    Log::wr(text.c_str());
}
void Log::er(const string &text){
    Log::er(text.c_str());
}

void Log::pr(const char * text){
    mutex.lock();

    printf("%s", text);

    mutex.unlock();
}


void Log::wr(const char * text){
    time_t rawtime;
    struct tm * timeinfo;
    char stime [30]; char * pstime = &stime[0];
    time ( &rawtime );
    timeinfo = localtime ( &rawtime );
    strftime (stime,30,"[%d.%m.%Y %H:%M:%S]",timeinfo);

    mutex.lock();

    printf("%s %s\n", pstime, text);

    FILE* f = fopen(app.conf.log_file.c_str(),  "a");
    if (f != 0)
    {
        fprintf(f, "%s %s\n", pstime, text);
        fclose(f);
    }

    mutex.unlock();
}
void Log::er(const char * text){
    time_t rawtime;
    struct tm * timeinfo;
    char stime [30]; char * pstime = &stime[0];
    time ( &rawtime );
    timeinfo = localtime ( &rawtime );
    strftime (stime,30,"[%d.%m.%Y %H:%M:%S]",timeinfo);

    mutex.lock();

    printf("%s %s\n", pstime, text);

    FILE* f = fopen(app.conf.log_file.c_str(),  "a");
    if (f != 0)
    {
        fprintf(f, "%s %s\n", pstime, text);
        fclose(f);
    }

    FILE* f2 = fopen(app.conf.err_log_file.c_str(),  "a");
    if (f2 != 0)
    {
        fprintf(f2, "%s %s\n", pstime, text);
        fclose(f2);
    }

    mutex.unlock();
}
