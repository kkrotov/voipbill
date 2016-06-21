#pragma once

#include "BasePull.h"

class PullTrunkPriority : public BasePull {
public:
    void init() {
        event = "trunk_priority";
        src_table = "auth.trunk_priority";
        dst_table = "auth.trunk_priority";

        key = "id";

        fields.push_back("id");
        fields.push_back("trunk_id");
        fields.push_back("order");
        fields.push_back("priority");
        fields.push_back("prefixlist_id");

        datatype.push_back("id integer");
        datatype.push_back("trunk_id integer");
        datatype.push_back("\"order\" smallint");
        datatype.push_back("priority smallint");
        datatype.push_back("prefixlist_id integer");
    }
};
