#pragma once

#include "../common.h"

struct Organization {
    int id;
    int organization_id;
    time_t actual_from;
    time_t actual_to;
    double vat_rate;

    void dump(stringstream &trace) {
        trace << "(";
        trace << "id: " << id << ", ";
        trace << "actual_from: " << string_time(actual_from) << ", ";
        trace << "actual_to: " << string_time(actual_to) << ", ";
        trace << "vat_rate: " << vat_rate << ", ";
        trace << ")";
    }
};
