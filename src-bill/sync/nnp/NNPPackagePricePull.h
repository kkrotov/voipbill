#pragma once

#include "../BasePull.h"

class NNPPackagePricePull : public BasePull {

public:
    void init() {

        event = "nnp_package_price";
        src_table = "nnp.package_price";
        dst_table = "nnp.package_price";

        key = "id";

        fields.push_back("id");
        fields.push_back("tariff_id");
        fields.push_back("destination_id");
        fields.push_back("price");

        datatype.push_back("id integer");
        datatype.push_back("tariff_id integer");
        datatype.push_back("destination_id integer");
        datatype.push_back("price double precision");
    }
};
