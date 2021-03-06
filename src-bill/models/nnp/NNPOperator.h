#pragma once

#include "../../common.h"

struct NNPOperator {
    int id;
    string name;

    void dump(stringstream &trace) {
        trace << "(";
        trace << "id: " << id << ", ";
        trace << "name: " << name << ", ";
        trace << ")";
    }
};