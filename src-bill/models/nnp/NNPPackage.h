#pragma once

#include "../../common.h"

struct NNPPackage {
    int id;
    int service_type_id;
    int tarification_free_seconds;
    int tarification_interval_seconds;
    int tarification_type;
    int tarification_min_paid_seconds;
    char currency_id[4];

    void dump(stringstream &trace) {
        trace << "(";
        trace << "id: " << id << ", ";
        trace << "service_type_id: " << service_type_id << ", ";
        trace << "currency_id: " << currency_id << ", ";
        trace << "tarification_free_seconds: " << tarification_free_seconds << ", ";
        trace << "tarification_interval_seconds: " << tarification_interval_seconds << ", ";
        trace << "tarification_type: " << tarification_type << ", ";
        trace << "tarification_min_paid_seconds: " << tarification_min_paid_seconds << ", ";
        trace << ")";
    }
};