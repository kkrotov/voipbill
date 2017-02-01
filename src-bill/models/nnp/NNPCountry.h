#pragma once

#include "../../common.h"

class NNPCountry {

public:
    int code;
    string name;
    std::set<int> prefixes;

    void dump(stringstream &trace) {

        trace << "(";
        trace << "code: " << code << ", ";
        trace << "name: " << name << ", ";
//        trace << "prefix: " << prefixes << "";
        trace << ")";
    }
};
