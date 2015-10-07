#pragma once

#include "../common.h"

#define STAT_PREFIXLIST_TYPE_MANUAL     1
#define STAT_PREFIXLIST_TYPE_ROSLINK    3

#define STAT_PREFIXLIST_SUBTYPE_ALL     0
#define STAT_PREFIXLIST_SUBTYPE_FIXED   1
#define STAT_PREFIXLIST_SUBTYPE_MOBILE  2

struct StatPrefixlist
{
    int id;
    int type_id;
    vector<long long int> prefixes;
    int sub_type;
    int country_id;
    int region_id;
    int city_id;
    bool exclude_operators;
    vector<int> operators;

    void dump(stringstream &trace) {
        trace << "(";
        trace << "id: " << id << ", ";
        trace << "type_id: " << type_id << ", ";
        trace << "country_id: " << country_id << ", ";
        trace << "region_id: " << region_id << ", ";
        trace << "city_id: " << city_id << ", ";
        trace << "exclude_operators: " << exclude_operators << ", ";
        trace << ")";
    }
};
