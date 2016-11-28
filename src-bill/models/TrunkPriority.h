#pragma once

#include "../common.h"

struct TrunkPriority {
    int trunk_id;
    int order;
    int priority;

    int number_id_filter_a;
    int number_id_filter_b;

    TrunkPriority() {
        trunk_id = 0;
        order = 0;
        priority = 0;
        number_id_filter_a = 0;
        number_id_filter_a = 0;
    }

    void dump(stringstream &trace) {
        trace << "(";
        trace << "trunk_id: " << trunk_id << ", ";
        trace << "order: " << order << ", ";
        trace << "priority: " << priority << ", ";
        trace << "number_id_filter_a: " << number_id_filter_a << ", ";
        trace << "number_id_filter_b: " << number_id_filter_b << ", ";
        trace << ")";
    }

};
