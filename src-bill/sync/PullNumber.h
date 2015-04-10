#pragma once

#include "BasePull.h"

class PullNumber : public BasePull {
public:
    void init() {
        event = "number";
        src_table = "auth.number";
        dst_table = "auth.number";

        key = "id";

        fields.push_back("id");
        fields.push_back("server_id");
        fields.push_back("name");
        fields.push_back("prefixlist_ids");
    }
};
