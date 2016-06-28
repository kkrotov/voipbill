#pragma once

#include "BasePull.h"

class PullTrunkTrunkRule : public BasePull {
public:
    void init() {
        event = "trunk_trunk_rule";
        src_table = "auth.trunk_trunk_rule";
        dst_table = "auth.trunk_trunk_rule";

        fields.push_back("trunk_id");
        fields.push_back("order");
        fields.push_back("trunk_group_id");

        datatype.push_back("trunk_id integer");
        datatype.push_back("\"order\" smallint");
        datatype.push_back("trunk_group_id integer");
    }
};
