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
};
