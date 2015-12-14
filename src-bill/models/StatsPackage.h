#pragma once

#include "../common.h"

struct StatsPackage {
    int id;
    int package_id;
    time_t activation_dt;
    time_t expire_dt;
    int used_seconds;
    double used_credit;
    int paid_seconds;
    long long int min_call_id;
    long long int max_call_id;

    void dump(stringstream &trace) {
        trace << "(";
        trace << "id: " << id << ", ";
        trace << "package_id: " << package_id << ", ";
        trace << "activation_dt: " << string_time(activation_dt) << ", ";
        trace << "expire_dt: " << string_time(expire_dt) << ", ";
        trace << "used_seconds: " << used_seconds << ", ";
        trace << "used_credit: " << used_credit << ", ";
        trace << "paid_seconds: " << paid_seconds << ", ";
        trace << "min_call_id: " << min_call_id << ", ";
        trace << "max_call_id: " << max_call_id << ", ";
        trace << ")";
    }
};
