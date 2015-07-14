#pragma once

#include "../common.h"

struct MobPrefix {
    char prefix[20];
    bool mob;

    void dump(stringstream &trace) {
        trace << "(";
        trace << "prefix: " << prefix << ", ";
        trace << "mob: " << mob << ", ";
        trace << ")";
    }
};