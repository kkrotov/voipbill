#pragma once

#include "BasePull.h"

class PullOrganization : public BasePull {
public:
    void init() {
        event = "organization";
        src_table = "billing.organization";
        dst_table = "billing.organization";

        key = "id";

        fields.push_back("id");
        fields.push_back("organization_id");
        fields.push_back("actual_from");
        fields.push_back("actual_to");
        fields.push_back("tax_rate");

        datatype.push_back("id integer");
        datatype.push_back("organization_id integer");
        datatype.push_back("actual_from date");
        datatype.push_back("actual_to date");
        datatype.push_back("tax_rate double precision");
    }
};
