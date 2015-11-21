#pragma once

#include "BasePull.h"

class PullStatPrefixlist : public BasePull {
public:
    void init() {
        event = "stat_prefixlist";
        src_table = "billing.stat_prefixlist";
        dst_table = "billing.stat_prefixlist";

        key = "id";

        fields.push_back("id");
        fields.push_back("type_id");
        fields.push_back("prefixes");
        fields.push_back("sub_type");
        fields.push_back("country_id");
        fields.push_back("region_id");
        fields.push_back("city_id");
        fields.push_back("exclude_operators");
        fields.push_back("operators");
    }
};
