#pragma once

#include "../common.h"

#define SERVICE_TRUNK_SETTINGS_ORIGINATION 1
#define SERVICE_TRUNK_SETTINGS_TERMINATION 2
#define SERVICE_TRUNK_SETTINGS_DESTINATION 3

struct ServiceTrunkSettings {
    int trunk_id;
    int type;
    int order;
    int src_number_id;
    int dst_number_id;
    int pricelist_id;

    void dump(stringstream &trace) {
        trace << "(";
        trace << "trunk_id: " << trunk_id << ", ";
        trace << "type: " << type << ", ";
        trace << "order: " << order << ", ";
        trace << "src_number_id: " << src_number_id << ", ";
        trace << "dst_number_id: " << dst_number_id << ", ";
        trace << "pricelist_id: " << pricelist_id << ", ";
        trace << ")";
    }
};