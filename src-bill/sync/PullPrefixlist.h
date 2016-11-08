#pragma once

#include "BasePull.h"

class PullPrefixlist : public BasePull {
public:
    void init() {
        event = "prefixlist";
        src_table = "auth.prefixlist";
        dst_table = "auth.prefixlist";

        key = "id";

        fields.push_back("id");
        fields.push_back("server_id");
        fields.push_back("name");
            fields.push_back("sw_shared");

        datatype.push_back("id integer");
        datatype.push_back("server_id integer");
        datatype.push_back("name character varying(50)");
            datatype.push_back("sw_shared boolean");
    }
};
