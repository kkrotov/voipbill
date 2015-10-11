#pragma once

#include "../common.h"

struct PrefixlistPrefix {
    int prefixlist_id;
    char prefix[51];

    void dump(stringstream &trace) {
        trace << "(";
        trace << "prefixlist_id: " << prefixlist_id << ", ";
        trace << "prefix: " << prefix << ", ";
        trace << ")";
    }
};
