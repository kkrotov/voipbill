#pragma once

#include "BasePull.h"

class PullStatDestinationPrefixlists : public BasePull {
public:
    void init() {
        event = "stat_destination_prefixlists";
        src_table = "billing.stat_destination_prefixlists";
        dst_table = "billing.stat_destination_prefixlists";

        key = "id";

        fields.push_back("id");
        fields.push_back("destination_id");
        fields.push_back("prefixlist_id");
    }
};
