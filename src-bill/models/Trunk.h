#pragma once

struct Trunk {
    int id;
    char name[51];
    char trunk_name[33];
    int code;
    bool source_rule_default_allowed;
    bool destination_rule_default_allowed;
    int default_priority;
    bool auto_routing;
    int route_table_id;
    bool our_trunk;
    bool auth_by_number;

    void dump(stringstream &trace) {
        trace << "(";
        trace << "id: " << id << ", ";
        trace << "trunk_name: " << trunk_name << ", ";
        trace << "code: " << code << ", ";
        trace << "source_rule_default_allowed: " << source_rule_default_allowed << ", ";
        trace << "destination_rule_default_allowed: " << destination_rule_default_allowed << ", ";
        trace << "default_priority: " << default_priority << ", ";
        trace << "auto_routing: " << auto_routing << ", ";
        trace << "route_table_id: " << route_table_id << ", ";
        trace << "our_trunk: " << our_trunk << ", ";
        trace << "auth_by_number: " << auth_by_number << ", ";
        trace << ")";
    }
};