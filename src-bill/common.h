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
#include <set>
using namespace std;

#include <boost/lexical_cast.hpp>
using boost::lexical_cast;

string string_fmt(const string &fmt, ...);

string string_date(const time_t dt);

string string_time(const time_t dt);

time_t parseDate(char * str);

time_t parseDateTime(const char * str);

time_t get_tday(time_t timestamp, short timezone_offset = 0);
time_t get_tday_end(time_t timestamp, short timezone_offset = 0);

time_t get_tmonth(time_t timestamp, short timezone_offset = 0);
time_t get_tmonth_end(time_t timestamp, short timezone_offset = 0);


char const * build_date();
char const * build_time();