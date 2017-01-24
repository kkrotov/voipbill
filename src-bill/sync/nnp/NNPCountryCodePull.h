#pragma once

#include "../BasePull.h"

class NNPCountryCodePull : public BasePull {

public:
    void init() {

        event = "nnp_country";
        src_table = "nnp.country";
        dst_table = "nnp.country";

        key = "code";

        fields.push_back("code");
        fields.push_back("name");
        fields.push_back("name_rus");
        fields.push_back("prefix");

        datatype.push_back("code integer");
        datatype.push_back("name varchar");
        datatype.push_back("name_rus varchar");
        datatype.push_back("prefix integer");
    }
};
