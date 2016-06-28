#pragma once

#include "BasePull.h"

class PullTariff : public BasePull {
public:
    void init() {
        event = "tariff";
        src_table = "billing.tariff";
        dst_table = "billing.tariff";

        key = "id";

        fields.push_back("id");
        fields.push_back("freemin");
        fields.push_back("freemin_for_number");
        fields.push_back("pricelist_id");
        fields.push_back("paid_redirect");
        fields.push_back("tariffication_by_minutes");
        fields.push_back("tariffication_full_first_minute");
        fields.push_back("tariffication_free_first_seconds");

        datatype.push_back("id smallint");
        datatype.push_back("freemin integer");
        datatype.push_back("freemin_for_number boolean");
        datatype.push_back("pricelist_id smallint");
        datatype.push_back("paid_redirect boolean");
        datatype.push_back("tariffication_by_minutes boolean");
        datatype.push_back("tariffication_full_first_minute boolean");
        datatype.push_back("tariffication_free_first_seconds boolean");
    }
};
