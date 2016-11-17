#pragma once

#include "../../common.h"

struct NNPPackageMinute {
    int id;
    int nnp_tariff_id;
    int nnp_destination_id;
    double minute;

    void dump(stringstream &trace) {
        trace << "(";
        trace << "id: " << id << ", ";
        trace << "nnp_tariff_id: " << nnp_tariff_id << ", ";
        trace << "nnp_destination_id: " << nnp_destination_id << ", ";
        trace << "minute: " << minute << ", ";
        trace << ")";
    }
};