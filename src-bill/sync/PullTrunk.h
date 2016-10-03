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
        fields.push_back("source_trunk_rule_default_allowed");
        fields.push_back("default_priority");
        fields.push_back("auto_routing");
        fields.push_back("route_table_id");
        fields.push_back("our_trunk");
        fields.push_back("auth_by_number");
        fields.push_back("orig_redirect_number_7800");
        fields.push_back("orig_redirect_number");
        fields.push_back("term_redirect_number");
        fields.push_back("capacity");

        datatype.push_back("id integer");
        datatype.push_back("server_id integer");
        datatype.push_back("trunk_name character varying(32)");
        datatype.push_back("trunk_name_alias character varying(32)");
        datatype.push_back("code smallint");
        datatype.push_back("name character varying(50)");
        datatype.push_back("source_rule_default_allowed boolean");
        datatype.push_back("destination_rule_default_allowed boolean");
        datatype.push_back("source_trunk_rule_default_allowed boolean");
        datatype.push_back("default_priority smallint");
        datatype.push_back("auto_routing boolean");
        datatype.push_back("route_table_id integer");
        datatype.push_back("our_trunk boolean");
        datatype.push_back("auth_by_number boolean");
        datatype.push_back("orig_redirect_number_7800 boolean");
        datatype.push_back("orig_redirect_number boolean");
        datatype.push_back("term_redirect_number boolean");
        datatype.push_back("capacity integer");
    }
};
