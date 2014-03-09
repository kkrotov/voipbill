#include "common.h"
#include "classes/App.h"
#include "classes/Logger.h"
#include "classes/Log.h"

#include <stdio.h>
#include <stdarg.h>

#define UNIXTIME_40000101 64060588800

string string_fmt(const string &fmt, ...) {
    int size = 100;
    string str;
    va_list ap;
    while (1) {
        str.resize(size);
        va_start(ap, fmt);
        int n = vsnprintf((char *) str.c_str(), size, fmt.c_str(), ap);
        va_end(ap);
        if (n > -1 && n < size) {
            str.resize(n);
            return str;
        }
        if (n > -1)
            size = n + 1;
        else
            size *= 2;
    }
}

string string_date(const time_t dt) {
    char buff[20];

    if (dt > UNIXTIME_40000101) {
        Log::error("string_date: bad unix time");
        return string("4000-01-01");
    }

    struct tm * timeinfo = localtime(&dt);
    if (timeinfo->tm_year < 0 || timeinfo->tm_year > 1000) {
        timeinfo->tm_year = 0;
        timeinfo->tm_mon = 0;
        timeinfo->tm_mday = 1;
    }
    strftime(buff, 20, "%Y-%m-%d", timeinfo);
    return string(buff);
}

string string_time(const time_t dt) {
    char buff[40];

    if (dt > UNIXTIME_40000101) {
        Log::error("string_time: bad unix time");
        return string("4000-01-01 00:00:00");
    }

    struct tm * timeinfo = localtime(&dt);
    if (timeinfo->tm_year < 0 || timeinfo->tm_year > 1000) {
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

time_t parseDate(char * str) {
    struct tm ttt;
    if (sscanf(str, "%d-%d-%d",
            &ttt.tm_year, &ttt.tm_mon, &ttt.tm_mday) > 0
            ) {
        ttt.tm_year -= 1900;
        ttt.tm_mon -= 1;
        ttt.tm_hour = 0;
        ttt.tm_min = 0;
        ttt.tm_sec = 0;
        ttt.tm_isdst = 0;
        ttt.tm_yday = 0;
        return mktime(&ttt);
    } else {
        return 0;
    }
}

time_t parseDateTime(char * str) {
    struct tm ttt;
    if (sscanf(str, "%d-%d-%d %d:%d:%d",
            &ttt.tm_year, &ttt.tm_mon, &ttt.tm_mday,
            &ttt.tm_hour, &ttt.tm_min, &ttt.tm_sec) > 0
            ) {
        ttt.tm_year -= 1900;
        ttt.tm_mon -= 1;
        ttt.tm_isdst = 0;
        ttt.tm_yday = 0;
        return mktime(&ttt);
    } else {
        return 0;
    }
}

bool parseDateTime(char * str, DT &dt) {
    struct tm ttt;
    if (sscanf(str, "%d-%d-%d %d:%d:%d",
            &ttt.tm_year, &ttt.tm_mon, &ttt.tm_mday,
            &ttt.tm_hour, &ttt.tm_min, &ttt.tm_sec) > 0
            ) {
        ttt.tm_year -= 1900;
        ttt.tm_mon -= 1;
        ttt.tm_isdst = 0;
        ttt.tm_yday = 0;
        dt.time = mktime(&ttt);
        dt.day = dt.time - ttt.tm_hour * 3600 - ttt.tm_min * 60 - ttt.tm_sec;
        dt.month = dt.day - (ttt.tm_mday - 1)*86400;
        return true;
    } else {
        dt.time = 0;
        dt.day = 0;
        dt.month = 0;
        Log::error("parseDateTime: can't parse '" + string(str) + "'");
        return false;
    }
}

time_t t_day_start = 0;
time_t t_day_end = 0;

time_t get_tday() {
    return get_tday(time(NULL));
}

time_t get_tday(const time_t rawtime) {

    if (rawtime >= t_day_start && rawtime <= t_day_end) {
        return t_day_start;
    }

    struct tm *ttt;
    ttt = localtime(&rawtime);
    ttt->tm_isdst = 0;
    ttt->tm_wday = 0;
    ttt->tm_yday = 0;
    ttt->tm_hour = 0;
    ttt->tm_min = 0;
    ttt->tm_sec = 0;
    t_day_start = mktime(ttt);
    t_day_end = t_day_start + 86400 - 1;
    return t_day_start;
}

time_t t_month_start = 0;
time_t t_month_end = 0;

time_t get_tmonth() {
    return get_tmonth(time(NULL));
}

time_t get_tmonth(const time_t rawtime) {

    if (rawtime >= t_month_start && rawtime <= t_month_end) {
        return t_month_start;
    }

    struct tm *ttt;
    ttt = localtime(&rawtime);
    ttt->tm_mday = 1;
    ttt->tm_isdst = 0;
    ttt->tm_wday = 0;
    ttt->tm_yday = 0;
    ttt->tm_hour = 0;
    ttt->tm_min = 0;
    ttt->tm_sec = 0;
    t_month_start = mktime(ttt);

    if (++ttt->tm_mon == 12) {
        ttt->tm_mon = 0;
        ttt->tm_year++;
    }
    t_month_end = mktime(ttt) - 1;
    return t_month_start;
}

time_t get_tmonth_end() {
    get_tmonth();
    return t_month_end;
}
