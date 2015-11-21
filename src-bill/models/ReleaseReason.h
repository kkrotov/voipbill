#pragma once

#include "../common.h"

struct ReleaseReason {
    int id;
    char name[51];

    void dump(stringstream &trace) {
        trace << "(";
        trace << "id: " << id << ", ";
        trace << "name: " << name << ", ";
        trace << ")";
    }
};
