#pragma once

#include "../common.h"

struct Trunk {
    int id;
    int server_id;
    char name[51];
    char trunk_name[33];
    int code;
    bool source_rule_default_allowed;
    bool destination_rule_default_allowed;
    bool source_trunk_rule_default_allowed;
    int default_priority;
    bool auto_routing;
    int route_table_id;
    bool our_trunk;
    bool auth_by_number;
    bool orig_redirect_number_7800;
    bool orig_redirect_number;
    bool term_redirect_number;
    bool sw_minimalki;
    bool sw_shared;
    bool tech_trunk;
    bool pstn_trunk;

    int capacity;
    int load_warning;
    vector<int> road_to_regions;
    bool is_connected(int region_id) { return std::find(road_to_regions.begin(), road_to_regions.end(), region_id ) != road_to_regions.end();  }

    void dump(stringstream &trace) {
        trace << "(";
        trace << "id: " << id << ", ";
        trace << "trunk_name: " << trunk_name << ", ";
        trace << "server_id: " << server_id << ", ";
        trace << "code: " << code << ", ";
        trace << "source_rule_default_allowed: " << source_rule_default_allowed << ", ";
        trace << "destination_rule_default_allowed: " << destination_rule_default_allowed << ", ";
        trace << "source_trunk_rule_default_allowed: " << source_trunk_rule_default_allowed << ", ";
        trace << "default_priority: " << default_priority << ", ";
        trace << "auto_routing: " << auto_routing << ", ";
        trace << "route_table_id: " << route_table_id << ", ";
        trace << "our_trunk: " << our_trunk << ", ";
        trace << "auth_by_number: " << auth_by_number << ", ";
        trace << "orig_redirect_number_7800: " << orig_redirect_number_7800 << ", ";
        trace << "orig_redirect_number: " << orig_redirect_number << ", ";
        trace << "term_redirect_number: " << term_redirect_number << ", ";
        trace << "capacity: " << capacity << ", ";
        trace << "sw_minimalki: " << sw_minimalki << ", ";
        trace << "sw_shared: " << sw_shared << ", ";
        trace << "tech_trunk: " << tech_trunk << ", ";
        trace << "pstn_trunk: " << pstn_trunk << ", ";
        trace << "load_warning: " << load_warning << ", ";
        std::stringstream result;
        std::copy(road_to_regions.begin(), road_to_regions.end(), std::ostream_iterator<int>(result, " "));
        trace << "road_to_region: " << result.str() << ", ";
        trace << ")";
    }
};
