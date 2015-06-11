#pragma once

struct GeoPrefix {
    char prefix[20];
    int geo_id;
    int geo_operator_id;


    void dump(stringstream &trace) {
        trace << "(";
        trace << "prefix: " << prefix << ", ";
        trace << "geo_id: " << geo_id << ", ";
        trace << "geo_operator_id: " << geo_operator_id << ", ";
        trace << ")";
    }
};