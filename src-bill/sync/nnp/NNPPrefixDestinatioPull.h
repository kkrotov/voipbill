#pragma once

#include "../BasePull.h"

class NNPPrefixDestinatioPull : public BasePull {

public:
    void init() {

        event = "nnp_prefix_destination";
        src_table = "nnp.prefix_destination";
        dst_table = "nnp.prefix_destination";

        key = "prefix_id";

        fields.push_back("prefix_id");
        fields.push_back("destination_id");
        fields.push_back("is_addition");
        fields.push_back("insert_time");
        fields.push_back("insert_user_id");

        datatype.push_back("prefix_id integer");
        datatype.push_back("destination_id integer");
        datatype.push_back("is_addition boolean");
        datatype.push_back("insert_time timestamp(0)");
        datatype.push_back("insert_user_id integer");
    }
};
