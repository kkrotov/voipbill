#pragma once

#include "../../src/classes/Conf.h"

class ConfAuth : public Conf {
public:
    string db_main;
    string db_local;

    unsigned short web_port;

    unsigned short server_id;
    string str_server_id;

protected:
    virtual bool parse_config_variables(boost::property_tree::ptree &pt);
};



