#pragma once

#include "Conf.h"
#include <map>

class ConfBill : public Conf {
public:
    string db_main;
    string db_calls;
    double db_bandwidth_limit_mbits;

    uint16_t web_port;
    vector<string> active_threads;
    vector<string> skip_threads;

    uint16_t instance_id;
    string str_instance_id;

    uint16_t hub_id;
    string str_hub_id;

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
    int calls_raw_sync_delay;

    string sql_regions_list;

    string get_sql_regions_list() { return sql_regions_list; }

    void set_sql_regions_list(string sql) { sql_regions_list = sql; }

protected:
    virtual bool parse_config_variables(boost::property_tree::ptree &pt);
};


