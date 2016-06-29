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

        datatype.push_back("prefix character varying(11)");
        datatype.push_back("mob boolean");
    }
};
