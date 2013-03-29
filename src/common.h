#pragma once

#include <time.h>

#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/replace.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/join.hpp>
using boost::algorithm::split;
using boost::algorithm::replace_all;
using boost::algorithm::join;

#include <boost/bind.hpp>

#include <string>
#include <sstream>
#include <map>
#include <vector>
using namespace std;

#include <boost/lexical_cast.hpp>
using boost::lexical_cast;


string string_fmt(const string &fmt, ...);

string string_date(const time_t dt);

string string_time(const time_t dt);

time_t get_tday();

time_t get_tmonth();

time_t get_tmonth_end();
