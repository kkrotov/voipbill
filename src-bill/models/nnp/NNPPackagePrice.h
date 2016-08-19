#pragma once

#include "../../common.h"

struct NNPPackagePrice {
    int id;
    int nnp_tariff_id;
    int nnp_destination_id;
    double price;

    void dump(stringstream &trace) {
        trace << "(";
        trace << "id: " << id << ", ";
        trace << "nnp_tariff_id: " << nnp_tariff_id << ", ";
        trace << "nnp_destination_id: " << nnp_destination_id << ", ";
        trace << "price: " << price << ", ";
        trace << ")";
    }
};