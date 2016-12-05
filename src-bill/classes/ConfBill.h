#pragma once

#include "Conf.h"
#include <map>

class ConfBill : public Conf {

    typedef enum {
        SINGLEREGIONAL, MULTIREGIONAL, APIHOST
    } BillerMode;

protected:

    BillerMode billerMode;

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
    vector<int> call_sync_delay;
    vector<int> cdr_sync_delay;
    vector<int> call_save_delay;
    vector<int> cdr_proc_wait_count;
    vector<int> call_save_wait_count;
    vector<int> thread_error_count;
    vector<int> trunk_max_load;

    string sql_regions_list;

    string get_sql_regions_list() {
        return sql_regions_list;
    }

    string get_sql_regions_for_load_list_list() {
        if (isApiHostMode()) return "(select id from public.server)";
        else return sql_regions_list;
    }

    void set_sql_regions_list(string sql) {
        sql_regions_list = sql;
    }

    string get_sql_hub_id() {
        return hub_id > 0 ? to_string(hub_id) : "NULL";
    }

    bool isSingleRegionalMode() {
        return (billerMode == SINGLEREGIONAL);
    }

    bool isMultiRegionalMode() {
        return (billerMode == MULTIREGIONAL);
    }

    bool isApiHostMode() {
        return (billerMode == APIHOST);
    }

    string getBillerMode() {
        switch (billerMode) {
            case SINGLEREGIONAL :
                return "SINGLE-REGIONAL";
            case MULTIREGIONAL  :
                return "MULTI-REGIONAL";
            case APIHOST        :
                return "API-HOST";
        };
    }

    vector<int> get_int_vector(boost::property_tree::ptree &pt, string key);

protected:
    virtual bool parse_config_variables(boost::property_tree::ptree &pt);

    void parse_biller_mode(string mode);
};


