#pragma once

#include "../common.h"

struct Geo {
    int id;
    int country_id;
    int region_id;
    int city_id;

    void dump(stringstream &trace) {
        trace << "(";
        trace << "id: " << id << ", ";
        trace << "country_id: " << country_id << ", ";
        trace << "region_id: " << region_id << ", ";
        trace << "city_id: " << city_id << ", ";
        trace << ")";
    }
};
