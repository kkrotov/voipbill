#pragma once

#include "../BasePull.h"

class NNPPackagePull : public BasePull {

public:
    void init() {

        event = "nnp_package";
        src_table = "nnp.package";
        dst_table = "nnp.package";

        key = "tariff_id";

        fields.push_back("tariff_id");

        datatype.push_back("tariff_id integer");
    }
};
