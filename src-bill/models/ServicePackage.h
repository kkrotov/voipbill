#pragma once

#include "../common.h"

struct ServicePackage {
    int id;
    int service_number_id;
    int service_trunk_id;
    time_t activation_dt;
    time_t expire_dt;
    int tariff_package_id;
    bool periodical;

    void dump(stringstream &trace) {
        trace << "(";
        trace << "id: " << id << ", ";
        trace << "service_number_id: " << service_number_id << ", ";
        trace << "service_trunk_id: " << service_trunk_id << ", ";
        trace << "activation_dt: " << string_time(activation_dt) << ", ";
        trace << "expire_dt: " << string_time(expire_dt) << ", ";
        trace << "tariff_package_id: " << tariff_package_id << ", ";
        trace << "periodical: " << periodical << ", ";
        trace << ")";
    }
};