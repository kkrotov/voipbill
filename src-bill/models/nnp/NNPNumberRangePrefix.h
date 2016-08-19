#pragma once

#include "../../common.h"

struct NNPNumberRangePrefix {
    int nnp_prefix_id;
    int nnp_number_range_id;
    time_t insert_time;

    void dump(stringstream &trace) {
        trace << "(";
        trace << "nnp_prefix_id: " << nnp_prefix_id << ", ";
        trace << "nnp_number_range_id: " << nnp_number_range_id << ", ";
        trace << "insert_time: " << string_time(insert_time) << ", ";
        trace << ")";
    }
};