#pragma once

#include "BasePull.h"

class PullDefs: public BasePull {
public:
    void init() {
        event = "defs";
        src_table = "billing.defs";
        dst_table = "billing.defs";

        fields.push_back("ndef");
        fields.push_back("pricelist_id");
        fields.push_back("date_from");
        fields.push_back("deleted");
        fields.push_back("date_to");
        fields.push_back("price");
    }
};
