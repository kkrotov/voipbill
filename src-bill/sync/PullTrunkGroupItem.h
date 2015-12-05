#pragma once

#include "BasePull.h"

class PullTrunkGroupItem : public BasePull {
public:
    void init() {
        event = "trunk_group_item";
        src_table = "auth.trunk_group_item";
        dst_table = "auth.trunk_group_item";

        fields.push_back("trunk_group_id");
        fields.push_back("trunk_id");
    }
};
