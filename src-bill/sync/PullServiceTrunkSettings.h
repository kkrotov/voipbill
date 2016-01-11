#pragma once

#include "BasePull.h"

class PullServiceTrunkSettings : public BasePull {
public:
    void init() {
        event = "service_trunk_settings";
        src_table = "billing.service_trunk_settings";
        dst_table = "billing.service_trunk_settings";

        key = "id";

        fields.push_back("id");
        fields.push_back("trunk_id");
        fields.push_back("type");
        fields.push_back("order");
        fields.push_back("src_number_id");
        fields.push_back("dst_number_id");
        fields.push_back("pricelist_id");
        fields.push_back("minimum_minutes");
        fields.push_back("minimum_cost");
    }
};
