#pragma once

#include "BasePull.h"

class PullUsage: public BasePull {
public:
    void init() {
        event = "usage";
        src_table = "billing.usage";
        dst_table = "billing.usage";

        key = "id";

        fields.push_back("id");
        fields.push_back("client_id");
        fields.push_back("phone_num");
        fields.push_back("actual_from");
        fields.push_back("actual_to");
        fields.push_back("no_of_lines");
        fields.push_back("region");
    }
};
