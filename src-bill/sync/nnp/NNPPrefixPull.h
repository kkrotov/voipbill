#pragma once

#include "../BasePull.h"

class NNPPrefixPull : public BasePull {

public:
    void init() {

        event = "nnp_prefix";
        src_table = "nnp.prefix";
        dst_table = "nnp.prefix";

        key = "id";

        fields.push_back("id");
        fields.push_back("name");

        datatype.push_back("id integer");
        datatype.push_back("name character varying(255)");
    }
};
