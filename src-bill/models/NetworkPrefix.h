#pragma once

#include "../common.h"

struct NetworkPrefix {
    int network_config_id;
    char prefix[20];
    time_t date_from;
    time_t date_to;
    int network_type_id;

    void dump(stringstream &trace) {
        trace << "(";
        trace << "network_config_id: " << network_config_id << ", ";
        trace << "prefix: " << prefix << ", ";
        trace << "date_from: " << string_time(date_from) << ", ";
        trace << "date_to: " << string_time(date_to) << ", ";
        trace << "network_type_id: " << network_type_id << ", ";
        trace << ")";
    }
};
