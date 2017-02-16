#pragma once

#include "../common.h"

struct RouteTable {
    int id;
    int server_id;
    char name[51];

    void dump(stringstream &trace) {
        trace << "(";
        trace << "id: " << id << ", ";
        trace << "server_id: " << server_id << ", ";
        trace << "name: " << name << ", ";
        trace << ")";
    }
};
