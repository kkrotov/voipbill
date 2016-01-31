#pragma once

#include "Conf.h"
#include <map>

class ConfBill : public Conf {
public:
    string db_main;
    string db_calls;
    double db_bandwidth_limit_mbits;

    uint16_t web_port;

    uint16_t instance_id;
    string str_instance_id;

    string openca_udp_host;
    uint16_t openca_udp_port;
    uint16_t udp_openca_select_interval;
    uint16_t udp_force_finish_call_interval;

    uint16_t billing_free_seconds;

    uint16_t global_counters_select_interval;

    uint16_t radius_auth_port;
    uint16_t radius_acct_port;
    string radius_secret;

    std::string cdr_ftp_host;
    std::string cdr_ftp_user;
    std::string cdr_ftp_password;
    std::string cdr_ftp_dir;
    std::string cdr_nasip;
    int cdr_parcer_interval;

protected:
    virtual bool parse_config_variables(boost::property_tree::ptree &pt);
};


