#pragma once

#include "../BasePull.h"

class NNPNumberRangePull : public BasePull {

public:
    void init() {

        event = "nnp_number_range";
        src_table = "nnp.number_range";
        dst_table = "nnp.number_range";

        key = "id";

        fields.push_back("id");
        fields.push_back("country_prefix");
        fields.push_back("country_code");
        fields.push_back("ndc");
        fields.push_back("number_from");
        fields.push_back("number_to");
        fields.push_back("is_mob");
        fields.push_back("is_active");
        fields.push_back("operator_id");
        fields.push_back("region_id");
        fields.push_back("insert_time");
        fields.push_back("insert_user_id");
        fields.push_back("update_time");
        fields.push_back("update_user_id");
        fields.push_back("city_id");
            fields.push_back("full_number_from");
            fields.push_back("full_number_to");

        datatype.push_back("id integer");
        datatype.push_back("country_prefix integer");
        datatype.push_back("country_code integer");
        datatype.push_back("ndc integer");
        datatype.push_back("number_from integer");
        datatype.push_back("number_to integer");
        datatype.push_back("is_mob boolean");
        datatype.push_back("is_active boolean");
        datatype.push_back("operator_id integer");
        datatype.push_back("region_id integer");
        datatype.push_back("insert_time timestamp(0)");
        datatype.push_back("insert_user_id integer");
        datatype.push_back("update_time timestamp(0)");
        datatype.push_back("update_user_id integer");
        datatype.push_back("city_id integer");
            datatype.push_back("full_number_from bigint");
            datatype.push_back("full_number_to bigint");

    }
};
