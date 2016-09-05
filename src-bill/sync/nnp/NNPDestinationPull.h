#pragma once

#include "../BasePull.h"

class NNPDestinationPull : public BasePull {

public:
    void init() {
        event = "nnp_destination";
        src_table = "nnp.destination";
        dst_table = "nnp.destination";

        key = "id";

        fields.push_back("id");
        fields.push_back("name");

        datatype.push_back("id integer");
        datatype.push_back("name character varying(255)");
    }
};
