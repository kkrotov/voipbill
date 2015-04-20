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
    }
};
