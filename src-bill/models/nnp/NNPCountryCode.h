#pragma once

#include "../../common.h"

class NNPCountryCode {

public:
    int code;
    string name;
    int prefix;

    void dump(stringstream &trace) {

        trace << "(";
        trace << "code: " << code << ", ";
        trace << "name: " << name << ", ";
        trace << "prefix: " << prefix << "";
        trace << ")";
    }
};
