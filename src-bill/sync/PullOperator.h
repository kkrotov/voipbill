#pragma once

#include "BasePull.h"

class PullOperator : public BasePull {
public:
    void init() {
        event = "operator";
        src_table = "voip.operator";
        dst_table = "billing.operator";

        fields.push_back("region");
        fields.push_back("id");
        fields.push_back("pricelist_id");
        fields.push_back("term_in_cost");
        fields.push_back("local_network_pricelist_id");
        fields.push_back("client_7800_pricelist_id");
        fields.push_back("operator_7800_pricelist_id");
    }
};
