#pragma once

#include "BasePull.h"

class PullServiceNumber : public BasePull {
public:
    void init() {
        event = "service_number";
        src_table = "billing.service_number";
        dst_table = "billing.service_number";

        key = "id";

        fields.push_back("id");
        fields.push_back("server_id");
        fields.push_back("client_account_id");
        fields.push_back("did");
        fields.push_back("activation_dt");
        fields.push_back("expire_dt");
        fields.push_back("lines_count");
        fields.push_back("tech_number");
        fields.push_back("tech_number_operator_id");

        datatype.push_back("id integer");
        datatype.push_back("server_id integer");
        datatype.push_back("client_account_id integer");
        datatype.push_back("did character varying(15)");
        datatype.push_back("activation_dt timestamp");
        datatype.push_back("expire_dt timestamp");
        datatype.push_back("lines_count integer");
        datatype.push_back("tech_number character varying(15)");
        datatype.push_back("tech_number_operator_id integer");
    }
};
