#pragma once

#include "BasePull.h"

class PullGeoPrefix : public BasePull {
public:
    void init() {
        event = "geo_prefix";
        src_table = "geo.prefix";
        dst_table = "geo.prefix";

        fields.push_back("prefix");
        fields.push_back("geo_id");
        fields.push_back("operator_id");

        datatype.push_back("prefix character varying(11)");
        datatype.push_back("geo_id integer");
        datatype.push_back("operator_id integer");
    }
};
