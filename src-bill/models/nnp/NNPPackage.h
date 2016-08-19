#pragma once

#include "../../common.h"

struct NNPPackage {
    int id;

    void dump(stringstream &trace) {
        trace << "(";
        trace << "id: " << id << ", ";
        trace << ")";
    }
};