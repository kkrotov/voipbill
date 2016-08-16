#pragma once

#include "../common.h"

struct TrunkTrunkRule {
    int trunk_id;
    int order;
    int trunk_group_id;
    int number_id_filter_a;
    int number_id_filter_b;

    TrunkTrunkRule() {
        trunk_id = 0;
        order = 0;
        trunk_group_id = 0;
        number_id_filter_a = 0;
        number_id_filter_a = 0;
    }

    TrunkTrunkRule(TrunkTrunkRule* rule) {
        trunk_id = rule->trunk_id;
        order = rule->order;
        trunk_group_id = rule->trunk_group_id;
        number_id_filter_a = rule->number_id_filter_a;
        number_id_filter_b = rule->number_id_filter_b;
    }

    void dump(stringstream &trace) {
        trace << "(";
        trace << "trunk_id: " << trunk_id << ", ";
        trace << "order: " << order << ", ";
        trace << "trunk_group_id: " << trunk_group_id << ", ";
        trace << "number_id_filter_a: " << number_id_filter_a << ", ";
        trace << "number_id_filter_b: " << number_id_filter_b << ", ";
        trace << ")";
    }

};
