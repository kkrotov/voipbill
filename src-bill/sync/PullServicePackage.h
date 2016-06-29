#pragma once

#include "BasePull.h"

class PullServicePackage : public BasePull {
public:
    void init() {
        event = "service_package";
        src_table = "billing.service_package";
        dst_table = "billing.service_package";

        key = "id";

        fields.push_back("id");
        fields.push_back("activation_dt");
        fields.push_back("expire_dt");
        fields.push_back("tariff_package_id");
        fields.push_back("service_number_id");
        fields.push_back("service_trunk_id");
        fields.push_back("periodical");

        datatype.push_back("id integer");
        datatype.push_back("activation_dt timestamp");
        datatype.push_back("expire_dt timestamp");
        datatype.push_back("tariff_package_id integer");
        datatype.push_back("service_number_id integer");
        datatype.push_back("service_trunk_id integer");
        datatype.push_back("periodical boolean");
    }
};
