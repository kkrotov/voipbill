#pragma once

#include "../../common.h"

struct NNPPrefixDestination {
    int nnp_prefix_id;
    int nnp_destination_id;
    bool is_addition;
    time_t insert_time;

    void dump(stringstream &trace) {
        trace << "(";
        trace << "nnp_prefix_id: " << nnp_prefix_id << ", ";
        trace << "nnp_destination_id: " << nnp_destination_id << ", ";
        trace << "insert_time: " << string_time(insert_time) << ", ";
        trace << ")";
    }
};