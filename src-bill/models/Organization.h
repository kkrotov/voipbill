#pragma once

struct Organization {
    int id;
    int organization_id;
    time_t actual_from;
    time_t actual_to;
    double tax_rate;

    void dump(stringstream &trace) {
        trace << "(";
        trace << "id: " << id << ", ";
        trace << "actual_from: " << string_time(actual_from) << ", ";
        trace << "actual_to: " << string_time(actual_to) << ", ";
        trace << "tax_rate: " << tax_rate << ", ";
        trace << ")";
    }
};
