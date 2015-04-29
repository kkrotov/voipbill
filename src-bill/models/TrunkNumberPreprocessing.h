#pragma once

struct TrunkNumberPreprocessing {
    int trunk_id;
    int order;
    bool src;
    int noa;
    int length;
    char prefix[11];

    void dump(stringstream &trace) {
        trace << "(";
        trace << "trunk_id: " << trunk_id << ", ";
        trace << "order: " << order << ", ";
        trace << "noa: " << noa << ", ";
        trace << "length: " << length << ", ";
        trace << "prefix: " << prefix << ", ";
        trace << ")";
    }
};
