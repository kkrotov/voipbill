#pragma once

#include "../common.h"

struct TrunkTrunkRule {
    int trunk_id;
    int order;
    int trunk_group_id;

    void dump(stringstream &trace) {
        trace << "(";
        trace << "trunk_id: " << trunk_id << ", ";
        trace << "order: " << order << ", ";
        trace << "trunk_group_id: " << trunk_group_id << ", ";
        trace << ")";
    }
};
