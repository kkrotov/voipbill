#pragma once

#include "../common.h"

#define SERVICE_TRUNK_SETTINGS_ORIGINATION 1
#define SERVICE_TRUNK_SETTINGS_TERMINATION 2
#define SERVICE_TRUNK_SETTINGS_DESTINATION 3

// Минимальная маржа не учитывается
#define SERVICE_TRUNK_SETTINGS_MIN_MARGIN_ABSENT  0
// Значение минимальной маржи в процентах
#define SERVICE_TRUNK_SETTINGS_MIN_MARGIN_PERCENT 1
// Значение минимальной маржи в единицах валюты
#define SERVICE_TRUNK_SETTINGS_MIN_MARGIN_VALUE   2

struct ServiceTrunkSettings {
    int id;
    int trunk_id;
    int type;
    int order;
    int src_number_id;
    int dst_number_id;
    int pricelist_id;
    int minimum_minutes;
    int minimum_cost;
    int nnp_tariff_id;

    // SERVICE_TRUNK_SETTINGS_MIN_MARGIN_...
    int minimum_margin_type;

    // Минимальная маржа в %, если
    //   minimum_margin_type = SERVICE_TRUNK_SETTINGS_MIN_MARGIN_PERCENT,
    // в рублях/долларах и т.п., если
    //   minimum_margin_type = SERVICE_TRUNK_SETTINGS_MIN_MARGIN_VALUE.
    double minimum_margin;

    void dump(stringstream &trace) {
        trace << "(";
        trace << "id: " << id << ", ";
        trace << "trunk_id: " << trunk_id << ", ";
        trace << "type: " << type << ", ";
        trace << "order: " << order << ", ";
        trace << "src_number_id: " << src_number_id << ", ";
        trace << "dst_number_id: " << dst_number_id << ", ";
        if(nnp_tariff_id>0) {
            trace << "pricelist_id: " << pricelist_id << ", ";
        }
        if(nnp_tariff_id>0) {
            trace << "nnp_tariff_id:" << nnp_tariff_id << ", ";
        }
        trace << "minimum_minutes: " << minimum_minutes << ", ";
        trace << "minimum_cost: " << minimum_cost << ", ";
        trace << "minimum_margin_type: " << minimum_margin_type << ", ";
        trace << "minimum_margin: " << minimum_margin << ", ";
        trace << ")";
    }
};
