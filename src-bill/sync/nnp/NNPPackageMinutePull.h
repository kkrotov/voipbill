#pragma once

#include "../BasePull.h"

class NNPPackageMinutePull : public BasePull {

public:
    void init() {

        event = "nnp_package_minute";
        src_table = "nnp.package_minute";
        dst_table = "nnp.package_minute";

        key = "id";

        fields.push_back("id");
        fields.push_back("tariff_id");
        fields.push_back("destination_id");
        fields.push_back("minute");

        datatype.push_back("id integer");
        datatype.push_back("tariff_id integer");
        datatype.push_back("destination_id integer");
        datatype.push_back("minute integer");
    }
};
