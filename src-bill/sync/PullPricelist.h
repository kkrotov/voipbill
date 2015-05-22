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
    }
};