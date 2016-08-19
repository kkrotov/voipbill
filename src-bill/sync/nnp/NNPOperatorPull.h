#pragma once

#include "../BasePull.h"

class NNPOperatorPull : public BasePull {

public:
    void init() {

        event = "nnp_operator";
        src_table = "nnp.operator";
        dst_table = "nnp.operator";

        key = "id";

        fields.push_back("id");
        fields.push_back("name");

        datatype.push_back("id integer");
        datatype.push_back("name character varying(255)");
    }
};
