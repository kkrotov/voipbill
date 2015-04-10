#pragma once

#include "BasePull.h"

class PullPrefixlist : public BasePull {
public:
    void init() {
        event = "prefixlist";
        src_table = "auth.prefixlist";
        dst_table = "auth.prefixlist";

        key = "id";

        fields.push_back("id");
        fields.push_back("server_id");
        fields.push_back("name");
    }
};
