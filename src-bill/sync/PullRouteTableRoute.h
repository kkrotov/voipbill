#pragma once

#include "BasePull.h"

class PullRouteTableRoute : public BasePull {
public:
    void init() {
        event = "route_table_route";
        src_table = "auth.route_table_route";
        dst_table = "auth.route_table_route";

        fields.push_back("route_table_id");
        fields.push_back("order");
        fields.push_back("a_number_id");
        fields.push_back("b_number_id");
        fields.push_back("outcome_id");
        fields.push_back("outcome_route_table_id");
    }
};
