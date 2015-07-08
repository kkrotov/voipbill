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
        fields.push_back("vat_rate");
    }
};
