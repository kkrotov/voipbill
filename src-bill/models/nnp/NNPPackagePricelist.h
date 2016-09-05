#pragma once

#include "../../common.h"

struct NNPPackagePricelist {
    int id;
    int nnp_tariff_id;
    int pricelist_id;

    void dump(stringstream &trace) {
        trace << "(";
        trace << "id: " << id << ", ";
        trace << "nnp_tariff_id: " << nnp_tariff_id << ", ";
        trace << "pricelist_id: " << pricelist_id << ", ";
        trace << ")";
    }
};