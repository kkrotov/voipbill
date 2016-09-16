#include "common.h"
#include "classes/App.h"
#include "classes/Logger.h"
#include "classes/Log.h"

#include <stdio.h>
#include <stdarg.h>

#define UNIXTIME_50010101 95649120000

string string_fmt(const string &fmt, ...) {
    int size = 100;
    string str;
    va_list ap;
    while (1) {
        str.resize((size_t)size);
        va_start(ap, fmt);
        int n = vsnprintf((char *) str.c_str(), (size_t)size, fmt.c_str(), ap);
        va_end(ap);
        if (n > -1 && n < size) {
            str.resize((size_t)n);
            return str;
        }
        if (n > -1)
            size = n + 1;
        else
            size *= 2;
    }
}

string string_date(const time_t dt, int debugFlag) {
    char buff[20];

    if (dt > UNIXTIME_50010101) {
        Log::error("string_date: bad unix time" + lexical_cast<string>(debugFlag));
        return string("5000-01-01");
    }

    struct tm timeinfo;
    gmtime_r(&dt, &timeinfo);
    if (timeinfo.tm_year < 0 || timeinfo.tm_year > 3000) {
        timeinfo.tm_year = 0;
        timeinfo.tm_mon = 0;
        timeinfo.tm_mday = 1;
    }
    strftime(buff, 20, "%Y-%m-%d", &timeinfo);
    return string(buff);
}

string string_time(const time_t dt, int debugFlag) {
    char buff[40];

    if (dt > UNIXTIME_50010101) {
        Log::error("string_time: bad unix time " + lexical_cast<string>(debugFlag));
        return string("5000-01-01 00:00:00");
    }

    struct tm timeinfo;
    gmtime_r(&dt, &timeinfo);
    if (timeinfo.tm_year < 0 || timeinfo.tm_year > 3000) {
        timeinfo.tm_year = 0;
        timeinfo.tm_mon = 0;
        timeinfo.tm_mday = 1;
        timeinfo.tm_hour = 0;
        timeinfo.tm_min = 0;
        timeinfo.tm_sec = 0;
    }
    strftime(buff, 40, "%Y-%m-%d %H:%M:%S", &timeinfo);
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
        return timegm(&ttt);
    } else {
        return 0;
    }
}

time_t parseDateTime(const char * str) {
    struct tm ttt;
    if (sscanf(str, "%d-%d-%d %d:%d:%d",
            &ttt.tm_year, &ttt.tm_mon, &ttt.tm_mday,
            &ttt.tm_hour, &ttt.tm_min, &ttt.tm_sec) > 0
            ) {
        ttt.tm_year -= 1900;
        ttt.tm_mon -= 1;
        ttt.tm_isdst = 0;
        ttt.tm_yday = 0;
        return timegm(&ttt);
    } else {
        return 0;
    }
}

time_t get_tday(time_t timestamp, short timezone_offset) {
    struct tm timeinfo;
    timestamp += 3600 * timezone_offset;
    gmtime_r(&timestamp, &timeinfo);
    timeinfo.tm_isdst = 0;
    timeinfo.tm_wday = 0;
    timeinfo.tm_yday = 0;
    timeinfo.tm_hour = 0;
    timeinfo.tm_min = 0;
    timeinfo.tm_sec = 0;
    return timegm(&timeinfo) - 3600 * timezone_offset;
}

time_t get_tday_end(time_t timestamp, short timezone_offset) {
    return get_tday(timestamp, timezone_offset) + 86399;
}

time_t get_tmonth(time_t timestamp, short timezone_offset) {
    struct tm timeinfo;
    timestamp += 3600 * timezone_offset;
    gmtime_r(&timestamp, &timeinfo);
    timeinfo.tm_isdst = 0;
    timeinfo.tm_wday = 0;
    timeinfo.tm_yday = 0;
    timeinfo.tm_hour = 0;
    timeinfo.tm_min = 0;
    timeinfo.tm_sec = 0;

    timeinfo.tm_mday = 1;

    return timegm(&timeinfo) - 3600 * timezone_offset;
}

time_t get_tmonth_end(time_t timestamp, short timezone_offset) {
    struct tm timeinfo;
    timestamp += 3600 * timezone_offset;
    gmtime_r(&timestamp, &timeinfo);
    timeinfo.tm_isdst = 0;
    timeinfo.tm_wday = 0;
    timeinfo.tm_yday = 0;
    timeinfo.tm_hour = 0;
    timeinfo.tm_min = 0;
    timeinfo.tm_sec = 0;

    timeinfo.tm_mday = 1;
    if (++timeinfo.tm_mon == 12) {
        timeinfo.tm_mon = 0;
        timeinfo.tm_year++;
    }

    return timegm(&timeinfo) - 1 - 3600 * timezone_offset;
}

/**
 * Removes html tags from the string.
 * Returns false if string was NOT modified.
 * Returns true if string was modified.
 *
 * @param str - string to remove tags from.
 *
 * @return false - string was NOT modified, true - string was modified.
 */
bool removeHtmlTags (string &str) {

    bool modified = false;
    stack<unsigned int> openIndex;

    if (!str.size())
        return false;

    for (int i = 0; i < str.size(); i++) {

        if (str.at(i) == '<') {
            openIndex.push (i);
            continue;
        }
        else if (str.at(i) == '>') {
            if ( openIndex.size() ) {
                for (int j = openIndex.top(); j <= i; j++)
                    str.at(j) = '\0';
                openIndex.pop();
                modified = true;
            }
            continue;
        }
    }

    if (!modified)
        return false;

    int dst = 0;
    for (int src = 0; src < str.size(); src++) {
        if (str.at(src)) {
            str.at(dst) = str.at(src);
            dst++;
        }
    }
    str.erase (dst, std::string::npos);

    return true;
}