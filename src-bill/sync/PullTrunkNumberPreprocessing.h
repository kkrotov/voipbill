#pragma once

#include "BasePull.h"

class PullTrunkNumberPreprocessing : public BasePull {
public:
    void init() {
        event = "trunk_number_preprocessing";
        src_table = "auth.trunk_number_preprocessing";
        dst_table = "auth.trunk_number_preprocessing";

        key = "id";

        fields.push_back("id");
        fields.push_back("trunk_id");
        fields.push_back("order");
        fields.push_back("src");
        fields.push_back("noa");
        fields.push_back("length");
        fields.push_back("prefix");

        datatype.push_back("id integer");
        datatype.push_back("trunk_id integer");
        datatype.push_back("\"order\" integer");
        datatype.push_back("src boolean");
        datatype.push_back("noa smallint");
        datatype.push_back("length smallint");
        datatype.push_back("prefix character varying(10)");
    }
};
