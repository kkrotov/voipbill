#pragma once

#include "BasePull.h"

class PullTrunk : public BasePull {
public:
    void init() {
        event = "trunk";
        src_table = "auth.trunk";
        dst_table = "auth.trunk";

        key = "id";

        fields.push_back("id");
        fields.push_back("server_id");
        fields.push_back("trunk_name");
        fields.push_back("trunk_name_alias");
        fields.push_back("code");
        fields.push_back("name");
        fields.push_back("source_rule_default_allowed");
        fields.push_back("destination_rule_default_allowed");
        fields.push_back("default_priority");
        fields.push_back("auto_routing");
        fields.push_back("route_table_id");
        fields.push_back("our_trunk");
        fields.push_back("auth_by_number");
    }
};
