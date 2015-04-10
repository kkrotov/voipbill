#pragma once

#include "BasePull.h"

class PullTariff : public BasePull {
public:
    void init() {
        event = "tariff";
        src_table = "billing.tarif";
        dst_table = "billing.tarif";

        key = "id";

        fields.push_back("id");
        fields.push_back("freemin");
        fields.push_back("freemin_for_number");
        fields.push_back("pricelist_id");
        fields.push_back("paid_redirect");
        fields.push_back("tariffication_by_minutes");
        fields.push_back("tariffication_full_first_minute");
        fields.push_back("tariffication_free_first_seconds");
    }
};
