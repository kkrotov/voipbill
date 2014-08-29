#pragma once

#include "../../src/classes/Conf.h"

class ConfBill : public Conf {
public:
    string db_main;
    string db_rad;
    string db_calls;

    unsigned short web_port;

    unsigned short instance_id;
    string str_instance_id;

    string udp_host;
    unsigned short udp_port;
    unsigned short udp_openca_select_interval;
    unsigned short udp_force_finish_call_interval;

    unsigned short billing_dc_break;
    unsigned short billing_free_seconds;
    
    unsigned short regions_counters_select_interval;

protected:
    virtual bool parse_config_variables(boost::property_tree::ptree &pt);
};


