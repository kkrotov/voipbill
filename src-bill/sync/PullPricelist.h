#pragma once

#include "BasePull.h"
#include "../classes/AppBill.h"

class PullPricelist : public BasePull {
public:
    void init() {
        event = "pricelist";
        src_table = "voip.pricelist";
        dst_table = "billing.pricelist";

        key = "id";

        fields.push_back("id");
        fields.push_back("region");
        fields.push_back("tariffication_by_minutes");
        fields.push_back("tariffication_full_first_minute");
        fields.push_back("orig");
        fields.push_back("local");
        fields.push_back("local_network_config_id");
        fields.push_back("initiate_mgmn_cost");
        fields.push_back("initiate_zona_cost");
        fields.push_back("price_include_vat");
        fields.push_back("currency_id");

        datatype.push_back("id smallint");
        datatype.push_back("region smallint");
        datatype.push_back("tariffication_by_minutes boolean");
        datatype.push_back("tariffication_full_first_minute boolean");
        datatype.push_back("orig boolean");
        datatype.push_back("local boolean");
        datatype.push_back("local_network_config_id integer");
        datatype.push_back("initiate_mgmn_cost numeric(8,2)");
        datatype.push_back("initiate_zona_cost numeric(8,2)");
        datatype.push_back("price_include_vat boolean");
        datatype.push_back("currency_id character(3)");

        string server_id = app().conf.str_instance_id;

        src_sql_where = " ( region  = " + server_id +" or is_global ) ";


    }
};
