#pragma once

#include "BasePull.h"

class PullHub : public BasePull {
public:
    void init() {
        event = "hub";
        src_table = "auth.hub";
        dst_table = "auth.hub";

        key = "id";

        fields.push_back("id");
        fields.push_back("name");

        datatype.push_back("id integer");
        datatype.push_back("name character varying(50)");
    }
};
