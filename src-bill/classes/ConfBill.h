#pragma once

#include "Conf.h"
#include <map>

class ConfBill : public Conf {
public:
    string db_main;
    string db_calls;

    unsigned short web_port;

    unsigned short instance_id;
    string str_instance_id;

    string openca_udp_host;
    unsigned short openca_udp_port;
    unsigned short udp_openca_select_interval;
    unsigned short udp_force_finish_call_interval;

    unsigned short billing_free_seconds;

    unsigned short global_counters_select_interval;

    std::string cdr_ftp_host;
    std::string cdr_ftp_user;
    std::string cdr_ftp_password;
    std::string cdr_ftp_dir;
    std::string cdr_nasip;
    int cdr_parcer_timeout;

protected:
    virtual bool parse_config_variables(boost::property_tree::ptree &pt);
};


