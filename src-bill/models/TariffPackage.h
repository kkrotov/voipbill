#pragma once

#include "../common.h"

struct TariffPackage {
    int id;
    int destination_id;
    int prepaid_minutes;
    int pricelist_id;

    int getPrepaidSeconds() {
        return prepaid_minutes * 60;
    }

    void dump(stringstream &trace) {
        trace << "(";
        trace << "id " << id << ", ";
        trace << "destination_id: " << destination_id << ", ";
        trace << "prepaid_minutes: " << prepaid_minutes << ", ";
        trace << "pricelist_id: " << pricelist_id << ", ";
        trace << ")";
    }
};
