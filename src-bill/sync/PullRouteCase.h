#pragma once

#include "BasePull.h"

class PullRouteCase : public BasePull {
public:
    void init() {
        event = "route_case";
        src_table = "auth.route_case";
        dst_table = "auth.route_case";

        key = "id";

        fields.push_back("id");
        fields.push_back("server_id");
        fields.push_back("name");
        fields.push_back("sw_shared");

        datatype.push_back("id integer");
        datatype.push_back("server_id integer");
        datatype.push_back("name character varying(50)");
        datatype.push_back("sw_shared boolean");
    }
};
