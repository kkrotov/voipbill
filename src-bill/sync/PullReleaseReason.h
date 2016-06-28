#pragma once

#include "BasePull.h"

class PullReleaseReason : public BasePull {
public:
    void init() {
        event = "release_reason";
        src_table = "auth.release_reason";
        dst_table = "auth.release_reason";

        key = "id";

        fields.push_back("id");
        fields.push_back("server_id");
        fields.push_back("name");

        datatype.push_back("id integer");
        datatype.push_back("server_id integer");
        datatype.push_back("name character varying(50)");
    }
};
