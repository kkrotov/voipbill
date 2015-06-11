#pragma once

#include "BasePull.h"

class PullInstance : public BasePull {
public:
    void init() {
        event = "instance_settings";
        src_table = "billing.instance_settings";
        dst_table = "billing.instance_settings";

        key = "id";

        fields.push_back("id");
        fields.push_back("region_id");
        fields.push_back("city_geo_id");
        fields.push_back("country_id");
        fields.push_back("city_id");
    }
};
