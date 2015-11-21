#pragma once

#include "../common.h"

struct RouteTableRoute {
    int route_table_id;
    int order;
    int a_number_id;
    int b_number_id;
    int outcome_id;
    int outcome_route_table_id;

    void dump(stringstream &trace) {
        trace << "(";
        trace << "route_table_id: " << route_table_id << ", ";
        trace << "order: " << order << ", ";
        trace << "a_number_id: " << a_number_id << ", ";
        trace << "b_number_id: " << b_number_id << ", ";
        trace << "outcome_id: " << outcome_id << ", ";
        trace << "outcome_route_table_id: " << outcome_route_table_id << ", ";
        trace << ")";
    }
};
