#pragma once

#include "../common.h"

struct Prefixlist {
    int id;
    char name[51];
    int type_id;
    int nnp_destination_id;

    void dump(stringstream &trace) {
        trace << "(";
        trace << "id: " << id << ", ";
        trace << "name: " << name << ", ";
        trace << "nnp_destination_id: " << nnp_destination_id << ", ";
        trace << "type_id: " << type_id;
        trace << ")";
    }
};
