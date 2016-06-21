#pragma once

#include "BasePull.h"

class PullServiceTrunk : public BasePull {
public:
    void init() {
        event = "service_trunk";
        src_table = "billing.service_trunk";
        dst_table = "billing.service_trunk";

        key = "id";

        fields.push_back("id");
        fields.push_back("server_id");
        fields.push_back("client_account_id");
        fields.push_back("trunk_id");
        fields.push_back("activation_dt");
        fields.push_back("expire_dt");
        fields.push_back("orig_enabled");
        fields.push_back("term_enabled");
        fields.push_back("orig_min_payment");
        fields.push_back("term_min_payment");

        datatype.push_back("id integer");
        datatype.push_back("server_id integer");
        datatype.push_back("client_account_id integer");
        datatype.push_back("trunk_id integer");
        datatype.push_back("activation_dt timestamp");
        datatype.push_back("expire_dt timestamp");
        datatype.push_back("orig_enabled boolean");
        datatype.push_back("term_enabled boolean");
        datatype.push_back("orig_min_payment double precision");
        datatype.push_back("term_min_payment double precision");
    }
};
