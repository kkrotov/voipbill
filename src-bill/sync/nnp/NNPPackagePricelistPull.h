#pragma once

#include "../BasePull.h"

class NNPPackagePricelistPull : public BasePull {

public:
    void init() {

        event = "nnp_package_pricelist";
        src_table = "nnp.package_pricelist";
        dst_table = "nnp.package_pricelist";

        key = "id";

        fields.push_back("id");
        fields.push_back("tariff_id");
        fields.push_back("pricelist_id");

        datatype.push_back("id integer");
        datatype.push_back("tariff_id integer");
        datatype.push_back("pricelist_id integer");
    }
};
