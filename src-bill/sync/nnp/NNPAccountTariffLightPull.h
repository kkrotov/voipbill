#pragma once

#include "../BasePull.h"

class NNPAccountTariffLightPull : public BasePull {

public:
    void init() {
        event = "nnp_account_tariff_light";
        src_table = "nnp.account_tariff_light";
        dst_table = "nnp.account_tariff_light";

        key = "id";

        fields.push_back("id");
            fields.push_back("account_tariff_id");
        fields.push_back("account_client_id");
        fields.push_back("tariff_id");
        fields.push_back("activate_from");
        fields.push_back("deactivate_from");
        fields.push_back("coefficient");
            fields.push_back("tariffication_by_minutes");
            fields.push_back("tariffication_full_first_minute");
            fields.push_back("tariffication_free_first_seconds");


        datatype.push_back("id integer");
            datatype.push_back("account_tariff_id integer");
        datatype.push_back("account_client_id integer");
        datatype.push_back("tariff_id integer");
        datatype.push_back("activate_from timestamp");
        datatype.push_back("deactivate_from timestamp");
        datatype.push_back("coefficient real");
            datatype.push_back("tariffication_by_minutes boolean");
            datatype.push_back("tariffication_full_first_minute boolean");
            datatype.push_back("tariffication_free_first_seconds boolean");


    }
};
