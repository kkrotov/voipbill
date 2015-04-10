#pragma once

#include "BasePull.h"

class PullClient : public BasePull {
public:
    void init() {
        event = "client";
        src_table = "billing.clients";
        dst_table = "billing.clients";

        key = "id";

        fields.push_back("id");
        fields.push_back("voip_limit_month");
        fields.push_back("voip_limit_day");
        fields.push_back("voip_disabled");
        fields.push_back("balance");
        fields.push_back("credit");
        fields.push_back("amount_date");
        fields.push_back("last_payed_month");
    }
};
