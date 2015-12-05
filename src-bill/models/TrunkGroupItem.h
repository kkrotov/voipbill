#pragma once

#include "../common.h"

struct TrunkGroupItem {
    int trunk_group_id;
    int trunk_id;

    void dump(stringstream &trace) {
        trace << "(";
        trace << "trunk_group_id: " << trunk_group_id << ", ";
        trace << "trunk_id: " << trunk_id << ", ";
        trace << ")";
    }
};
