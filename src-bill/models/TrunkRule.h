#pragma once

#include "../common.h"

struct TrunkRule {
    int trunk_id;
    bool outgoing;
    int order;
    int prefixlist_id;

    void dump(stringstream &trace) {
        trace << "(";
        trace << "trunk_id: " << trunk_id << ", ";
        trace << "outgoing: " << outgoing << ", ";
        trace << "order: " << order << ", ";
        trace << "prefixlist_id: " << prefixlist_id << ", ";
        trace << ")";
    }
};
