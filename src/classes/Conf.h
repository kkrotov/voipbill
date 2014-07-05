#pragma once

#include <string>
#include <vector>
using namespace std;

#include <boost/property_tree/ptree.hpp>

#include "BaseLogWriter.h"

class Conf {
public:
    string app_directory;
    string app_name;

    string config_file;
    string pid_file;

    string log_file_filename;
    LogLevel log_file_min_level;
    LogLevel log_file_max_level;

    string log_file2_filename;
    LogLevel log_file2_min_level;
    LogLevel log_file2_max_level;

    string log_syslog_ident;
    LogLevel log_syslog_min_level;
    LogLevel log_syslog_max_level;

    unsigned short log_grouping_interval;

    bool test_mode;

    bool init(int argc, char* argv[]);
protected:
    virtual bool parse_config_variables(boost::property_tree::ptree &pt) = 0;

private:
    bool parse_cmd_line(int argc, char* argv[]);
    bool parse_config_file();
    bool prepare();
};
