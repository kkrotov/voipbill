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
        fields.push_back("nnp_tariff_id");
        fields.push_back("minimum_minutes");
        fields.push_back("minimum_cost");
        fields.push_back("minimum_margin_type");
        fields.push_back("minimum_margin");

        datatype.push_back("id integer");
        datatype.push_back("trunk_id integer");
        datatype.push_back("type smallint");
        datatype.push_back("\"order\" smallint");
        datatype.push_back("src_number_id integer");
        datatype.push_back("dst_number_id integer");
        datatype.push_back("pricelist_id integer");
        datatype.push_back("nnp_tariff_id integer");
        datatype.push_back("minimum_minutes integer");
        datatype.push_back("minimum_cost integer");
        datatype.push_back("minimum_margin_type smallint");
        datatype.push_back("minimum_margin numeric(10,5)");
    }
};
