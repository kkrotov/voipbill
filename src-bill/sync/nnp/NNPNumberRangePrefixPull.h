#pragma once

#include "../BasePull.h"

class NNPNumberRangePrefixPull : public BasePull {

public:
    void init() {

        event = "nnp_number_range_prefix";
        src_table = "nnp.number_range_prefix";
        dst_table = "nnp.number_range_prefix";

        key = "prefix_id";

        fields.push_back("prefix_id");
        fields.push_back("number_range_id");
        fields.push_back("insert_time");
        fields.push_back("insert_user_id");

        datatype.push_back("prefix_id integer");
        datatype.push_back("number_range_id integer");
        datatype.push_back("insert_time timestamp(0)");
        datatype.push_back("insert_user_id integer");
    }
};