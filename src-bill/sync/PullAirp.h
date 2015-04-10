#pragma once

#include "BasePull.h"

class PullAirp : public BasePull {
public:
    void init() {
        event = "airp";
        src_table = "auth.airp";
        dst_table = "auth.airp";

        key = "id";

        fields.push_back("id");
        fields.push_back("server_id");
        fields.push_back("name");
    }
};
