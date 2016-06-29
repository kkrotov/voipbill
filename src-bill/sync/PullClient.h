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
        fields.push_back("organization_id");
        fields.push_back("price_include_vat");
        fields.push_back("timezone_offset");
        fields.push_back("is_blocked");
        fields.push_back("anti_fraud_disabled");

        datatype.push_back("id integer");
        datatype.push_back("voip_limit_month integer");
        datatype.push_back("voip_limit_day integer");
        datatype.push_back("voip_disabled boolean");
        datatype.push_back("balance numeric(12,2)");
        datatype.push_back("credit integer");
        datatype.push_back("amount_date timestamp(0)");
        datatype.push_back("last_payed_month date");
        datatype.push_back("organization_id integer");
        datatype.push_back("price_include_vat boolean");
        datatype.push_back("timezone_offset smallint");
        datatype.push_back("is_blocked boolean");
        datatype.push_back("anti_fraud_disabled boolean");
    }
};
