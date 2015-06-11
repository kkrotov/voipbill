#pragma once

#include "BasePull.h"

class PullGeo : public BasePull {
public:
    void init() {
        event = "geo";
        src_table = "geo.geo";
        dst_table = "geo.geo";

        key = "id";

        fields.push_back("id");
        fields.push_back("country");
        fields.push_back("region");
        fields.push_back("city");
    }
};
