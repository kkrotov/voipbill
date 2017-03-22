#pragma once

#include "../common.h"

struct Prefixlist {
    int id;
    char name[51];
    int type_id;
    int nnp_destination_id;
    int country_code;
    int operator_id;
    int region_id;
    int city_id;
    int ndc_type_id;
    string ndc_token;

    void dump(stringstream &trace) {
        trace << "(";
        trace << "id: " << id << ", ";
        trace << "name: " << name << ", ";
        trace << "nnp_destination_id: " << nnp_destination_id << ", ";
        trace << "type_id: " << type_id << ", ";
        trace << "country_code: " << country_code << ", ";
        trace << "operator_id: " << operator_id << ", ";
        trace << "region_id: " << region_id << ", ";
        trace << "city_id: " << city_id << ", ";
        trace << "ndc_type_id: " << ndc_type_id << ", ";
        trace << "ndc_token: " << ndc_token;
        trace << ")";
    }
};
