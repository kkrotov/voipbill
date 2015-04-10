#pragma once

#include "BasePull.h"

class PullPrefixlistPrefix : public BasePull {
public:
    void init() {
        event = "prefixlist_prefix";
        src_table = "auth.prefixlist_prefix";
        dst_table = "auth.prefixlist_prefix";

        fields.push_back("prefixlist_id");
        fields.push_back("prefix");
    }
};
