#pragma once

#include "BasePull.h"

class PullRoutingReportData : public BasePull {
public:
    void init() {
        event = "routing_report_data";
        src_table = "auth.routing_report_data";
        dst_table = "auth.routing_report_data";

        fields.push_back("prefix");
        fields.push_back("routes");

        datatype.push_back("prefix character varying(20)");
        datatype.push_back("routes integer[]");
    }
};
