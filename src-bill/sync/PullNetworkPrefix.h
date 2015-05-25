#pragma once

#include "BasePull.h"

class PullNetworkPrefix : public BasePull {
public:
    void init() {
        event = "network_prefix";
        src_table = "billing.network_prefix";
        dst_table = "billing.network_prefix";

        fields.push_back("prefix");
        fields.push_back("network_config_id");
        fields.push_back("date_from");
        fields.push_back("deleted");
        fields.push_back("date_to");
        fields.push_back("network_type_id");
    }
};
