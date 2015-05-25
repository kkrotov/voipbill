#pragma once

#include "BasePull.h"

class PullPricelist : public BasePull {
public:
    void init() {
        event = "pricelist";
        src_table = "voip.pricelist";
        dst_table = "billing.pricelist";

        key = "id";

        fields.push_back("id");
        fields.push_back("region");
        fields.push_back("operator_id");
        fields.push_back("tariffication_by_minutes");
        fields.push_back("tariffication_full_first_minute");
        fields.push_back("orig");
        fields.push_back("local");
        fields.push_back("local_network_config_id");
        fields.push_back("initiate_mgmn_cost");
        fields.push_back("initiate_zona_cost");
    }
};
