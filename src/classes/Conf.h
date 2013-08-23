#pragma once

#include <string>
#include <vector>
using namespace std;

class Conf {
    bool parse_cmd_line(int argc, char* argv[]);
    bool parse_config_file();
    bool prepare();

public:
    string app_directory;
    string app_name;

    string config_file;
    string pid_file;
    string log_file;
    string err_log_file;
    unsigned short log_grouping_interval;

    bool test_mode;
    unsigned short web_port;

    string db_main;
    string db_rad;
    string db_calls;

    unsigned short region_id;
    string str_region_id;

    string udp_host;
    unsigned short udp_port;

    unsigned short billing_dc_break;
    unsigned short billing_free_seconds;

    bool init(int argc, char* argv[]);


};
