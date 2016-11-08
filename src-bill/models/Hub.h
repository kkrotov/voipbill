#pragma once

#include "../common.h"


struct Hub {
    int id;
    string name;

    void dump(stringstream &trace) {
        trace << "(";
        trace << "id: " << id << ", ";
        trace << "name: " << name << ", ";
        trace << ")";
    }
};




