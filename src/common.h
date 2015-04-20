#pragma once

#include <time.h>

#include <boost/algorithm/string.hpp>

using boost::algorithm::split;
using boost::algorithm::replace_all;
using boost::algorithm::join;
using boost::algorithm::is_any_of;

#include <boost/bind.hpp>

#include <string>
#include <sstream>
#include <map>
#include <vector>
#include <queue>
#include <deque>
using namespace std;

#include <boost/lexical_cast.hpp>
using boost::lexical_cast;

typedef struct _DT {
    time_t time;
    time_t day;
    time_t month;
} DT, *pDT;

string string_fmt(const string &fmt, ...);

string string_date(const time_t dt);

string string_time(const time_t dt);

time_t parseDate(char * str);

time_t parseDateTime(char * str);

bool parseDateTime(char * str, DT &dt);

time_t get_tday();
time_t get_tday(const time_t rawtime);

time_t get_tmonth();
time_t get_tmonth(const time_t rawtime);

time_t get_tmonth_end();


char const * build_date();
char const * build_time();