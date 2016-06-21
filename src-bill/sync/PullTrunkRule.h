#pragma once

#include "BasePull.h"

class PullTrunkRule : public BasePull {
public:
    void init() {
        event = "trunk_rule";
        src_table = "auth.trunk_rule";
        dst_table = "auth.trunk_rule";

        fields.push_back("trunk_id");
        fields.push_back("outgoing");
        fields.push_back("order");
        fields.push_back("prefixlist_id");

        datatype.push_back("trunk_id integer");
        datatype.push_back("outgoing boolean");
        datatype.push_back("\"order\" smallint");
        datatype.push_back("prefixlist_id integer");
    }
};
