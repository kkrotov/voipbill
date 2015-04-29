#pragma once

struct GeoPrefix {
    char prefix[20];
    int dest;
    int geo_id;
    int geo_operator_id;
    int region_id;
    bool mob;


    void dump(stringstream &trace) {
        trace << "(";
        trace << "prefix: " << prefix << ", ";
        trace << "dest: " << dest << ", ";
        trace << "geo_id: " << geo_id << ", ";
        trace << "geo_operator_id: " << geo_operator_id << ", ";
        trace << "region_id: " << region_id << ", ";
        trace << "mob: " << mob << ", ";
        trace << ")";
    }
};