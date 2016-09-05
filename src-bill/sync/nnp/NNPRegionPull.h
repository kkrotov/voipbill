#pragma once

#include "../BasePull.h"

class NNPRegionPull : public BasePull {

public:
    void init() {

        event = "nnp_region";
        src_table = "nnp.region";
        dst_table = "nnp.region";

        key = "id";

        fields.push_back("id");
        fields.push_back("name");

        datatype.push_back("id integer");
        datatype.push_back("name character varying(255)");
    }
};
