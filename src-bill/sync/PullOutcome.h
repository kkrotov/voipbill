#pragma once

#include "BasePull.h"

class PullOutcome : public BasePull {
public:
    void init() {
        event = "outcome";
        src_table = "auth.outcome";
        dst_table = "auth.outcome";

        key = "id";

        fields.push_back("id");
        fields.push_back("server_id");
        fields.push_back("name");
        fields.push_back("calling_station_id");
        fields.push_back("called_station_id");
        fields.push_back("type_id");
        fields.push_back("route_case_id");
        fields.push_back("release_reason_id");
        fields.push_back("airp_id");
    }
};
