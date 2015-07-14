#pragma once

#include "BasePull.h"

class PullTariffPackage : public BasePull {
public:
    void init() {
        event = "tariff_package";
        src_table = "billing.tariff_package";
        dst_table = "billing.tariff_package";

        key = "id";

        fields.push_back("id");
        fields.push_back("destination_id");
        fields.push_back("prepaid_minutes");
        fields.push_back("pricelist_id");
    }
};
