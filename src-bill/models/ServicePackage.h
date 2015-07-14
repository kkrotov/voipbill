#pragma once

#include "../common.h"

struct ServicePackage {
    int id;
    int service_number_id;
    time_t activation_dt;
    time_t expire_dt;
    int tariff_package_id;
    long long int min_call_id;
    long long int max_call_id;

    void dump(stringstream &trace) {
        trace << "(";
        trace << "id: " << id << ", ";
        trace << "service_number_id: " << service_number_id << ", ";
        trace << "activation_dt: " << string_time(activation_dt) << ", ";
        trace << "expire_dt: " << string_time(expire_dt) << ", ";
        trace << "tariff_package_id: " << tariff_package_id << ", ";
        trace << "min_call_id: " << min_call_id << ", ";
        trace << "max_call_id: " << max_call_id << ", ";
        trace << ")";
    }
};