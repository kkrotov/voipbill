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
        fields.push_back("service_type_id");
        fields.push_back("tarification_free_seconds");
        fields.push_back("tarification_interval_seconds");
        fields.push_back("tarification_type");
        fields.push_back("tarification_min_paid_seconds");
        fields.push_back("currency_id");

        datatype.push_back("tariff_id integer");
        datatype.push_back("service_type_id integer");
        datatype.push_back("tarification_free_seconds integer");
        datatype.push_back("tarification_interval_seconds integer");
        datatype.push_back("tarification_type integer");
        datatype.push_back("tarification_min_paid_seconds integer");
        datatype.push_back("currency_id character(3)");
    }
};
