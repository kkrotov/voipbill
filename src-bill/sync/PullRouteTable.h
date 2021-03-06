#pragma once

#include "BasePull.h"

class PullRouteTable : public BasePull {
public:
    void init() {
        event = "route_table";
        src_table = "auth.route_table";
        dst_table = "auth.route_table";

        key = "id";

        fields.push_back("id");
        fields.push_back("server_id");
        fields.push_back("name");

        datatype.push_back("id integer");
        datatype.push_back("server_id integer");
        datatype.push_back("name character varying(50)");
    }
};
