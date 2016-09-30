#pragma once

#include "../common.h"

struct StatsNNPPackageMinute {
    int id;
    int nnp_account_tariff_light_id;
    int nnp_package_minute_id;
    time_t activate_from;
    time_t deactivate_from;
    int used_seconds;
    double used_credit;
    int paid_seconds;
    long long int min_call_id;
    long long int max_call_id;

    void dump(stringstream &trace) {
        trace << "(";
        trace << "id: " <<  id << ", ";
        trace << "nnp_account_tariff_light_id: " <<  nnp_account_tariff_light_id << ", ";
        trace << "nnp_package_minute_id: " <<   nnp_package_minute_id << ", ";
        trace << "activate_from: " << string_time(activate_from) << ", ";
        trace << "deactivate_from: " << string_time(deactivate_from) << ", ";
        trace << "used_seconds: " << used_seconds << ", ";
        trace << "used_credit: " << used_credit << ", ";
        trace << "paid_seconds: " << paid_seconds << ", ";
        trace << "min_call_id: " << min_call_id << ", ";
        trace << "max_call_id: " << max_call_id << ", ";
        trace << ")";
    }
};

