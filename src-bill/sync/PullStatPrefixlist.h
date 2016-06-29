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

        datatype.push_back("id integer");
        datatype.push_back("type_id smallint");
        datatype.push_back("prefixes text");
        datatype.push_back("sub_type character varying(10)");
        datatype.push_back("country_id integer");
        datatype.push_back("region_id integer");
        datatype.push_back("city_id integer");
        datatype.push_back("exclude_operators boolean");
        datatype.push_back("operators text");
    }
};
