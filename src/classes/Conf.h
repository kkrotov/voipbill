#pragma once

#include <string>
#include <vector>
using namespace std;

#include "BaseLogWriter.h"

class Conf {
    bool parse_cmd_line(int argc, char* argv[]);
    bool parse_config_file();
    bool prepare();

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
    unsigned short web_port;

    string db_main;
    string db_rad;
    string db_calls;

    unsigned short instance_id;
    string str_instance_id;

    string udp_host;
    unsigned short udp_port;
    unsigned short udp_openca_select_interval;
    unsigned short udp_force_finish_call_interval;

    unsigned short billing_dc_break;
    unsigned short billing_free_seconds;

    bool init(int argc, char* argv[]);


};
