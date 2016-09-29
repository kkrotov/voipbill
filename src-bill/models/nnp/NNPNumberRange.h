#pragma once

#include "../../common.h"

struct NNPNumberRange {
    int id;
    int ndc;
    long long int number_from;
    long long int number_to;
    long long int full_number_from;
    long long int full_number_to;
    bool is_mob;
    bool is_active;
    int nnp_operator_id;
    int nnp_region_id;
    time_t insert_time;
    time_t update_time;
    int nnp_city_id;
    int country_prefix;

    void dump(stringstream &trace) {
        trace << "(";
        trace << "id: " << id << ", ";
            trace << "country_prefix: " << country_prefix << ", ";
        trace << "ndc: " << ndc << ", ";
        trace << "number_from: " << number_from << ", ";
        trace << "number_to: " << number_to << ", ";
            trace << "full_number_from: " << full_number_from << ", ";
            trace << "full_number_to: " << full_number_to << ", ";
        trace << "is_mob: " << is_mob << ", ";
        trace << "is_active: " << is_active << ", ";
        trace << "nnp_operator_id: " << nnp_operator_id << ", ";
        trace << "nnp_region_id: " << nnp_region_id << ", ";
        trace << "nnp_city_id: " << nnp_city_id << ", ";
        trace << "insert_time: " << string_time(insert_time) << ", ";
        trace << "update_time: " << string_time(update_time) << ", ";
        trace << ")";
    }
};