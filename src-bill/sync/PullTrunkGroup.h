#pragma once

#include "BasePull.h"

class PullTrunkGroup : public BasePull {
public:
    void init() {
        event = "trunk_group";
        src_table = "auth.trunk_group";
        dst_table = "auth.trunk_group";

        key = "id";

        fields.push_back("id");
        fields.push_back("server_id");
        fields.push_back("name");
    }
};
