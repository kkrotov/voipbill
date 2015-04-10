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
    }
};
