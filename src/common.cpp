#include <string>
using namespace std;
#include <stdio.h>
#include <stdarg.h>

#ifdef _WIN32
  #define atoll(S) _atoi64(S)
#endif

#ifdef _WIN64
  #define atoll(S) _atoi64(S)
#endif

string string_fmt(const string &fmt, ...) {
    int size=100;
    string str;
    va_list ap;
    while (1) {
        str.resize(size);
        va_start(ap, fmt);
        int n = vsnprintf((char *)str.c_str(), size, fmt.c_str(), ap);
        va_end(ap);
        if (n > -1 && n < size) {
            str.resize(n);
            return str;
        }
        if (n > -1)
            size=n+1;
        else
            size*=2;
    }
}

string string_date(const time_t dt) {
    char buff[20];
    struct tm * timeinfo = localtime(&dt);
    if (timeinfo->tm_year < 0 || timeinfo->tm_year > 1000)
    {
        timeinfo->tm_year = 0;
        timeinfo->tm_mon = 0;
        timeinfo->tm_mday = 1;
    }
    strftime(buff, 20, "%Y-%m-%d", timeinfo);
    return string(buff);
}

string string_time(const time_t dt) {
    char buff[40];
    struct tm * timeinfo = localtime(&dt);
    if (timeinfo->tm_year < 0 || timeinfo->tm_year > 1000)
    {
        timeinfo->tm_year = 0;
        timeinfo->tm_mon = 0;
        timeinfo->tm_mday = 1;
        timeinfo->tm_hour = 0;
        timeinfo->tm_min = 0;
        timeinfo->tm_sec = 0;
    }
    strftime(buff, 40, "%Y-%m-%d %H:%M:%S", timeinfo);
    return string(buff);
}

time_t t_day_start = 0;
time_t t_day_end = 0;

time_t get_tday(){
    time_t rawtime = time(NULL);

    if (rawtime>=t_day_start && rawtime<=t_day_end)
    {
        return t_day_start;
    }

    struct tm *ttt;
    ttt = localtime(&rawtime);
    ttt->tm_isdst = 0; ttt->tm_wday = 0; ttt->tm_yday = 0;
    ttt->tm_hour = 0; ttt->tm_min = 0; ttt->tm_sec = 0;
    t_day_start = mktime(ttt);
    t_day_end = t_day_start + 86400 - 1;
    return t_day_start;
}

time_t t_month_start = 0;
time_t t_month_end = 0;

time_t get_tmonth(){
    time_t rawtime = time(NULL);

    if (rawtime>=t_month_start && rawtime<=t_month_end)
    {
        return t_month_start;
    }

    struct tm *ttt;
    ttt = localtime(&rawtime);
    ttt->tm_mday = 1;
    ttt->tm_isdst = 0; ttt->tm_wday = 0; ttt->tm_yday = 0;
    ttt->tm_hour = 0; ttt->tm_min = 0; ttt->tm_sec = 0;
    t_month_start = mktime(ttt);

    if (++ttt->tm_mon == 12){
        ttt->tm_mon = 0;
        ttt->tm_year++;
    }
    t_month_end = mktime(ttt) - 1;
    return t_month_start;
}

time_t get_tmonth_end(){
    get_tmonth();
    return t_month_end;
}
