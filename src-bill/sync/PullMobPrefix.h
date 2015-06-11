#pragma once

#include "BasePull.h"

class PullMobPrefix : public BasePull {
public:
    void init() {
        event = "mob_prefix";
        src_table = "geo.mob_prefix";
        dst_table = "geo.mob_prefix";

        fields.push_back("prefix");
        fields.push_back("mob");
    }
};
