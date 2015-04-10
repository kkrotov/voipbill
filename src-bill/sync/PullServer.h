#pragma once

#include "BasePull.h"

class PullServer: public BasePull {
public:
    void init() {
        event = "server";
        src_table = "public.server";
        dst_table = "public.server";

        key = "id";

        fields.push_back("id");
        fields.push_back("low_balance_outcome_id");
        fields.push_back("blocked_outcome_id");
        fields.push_back("calling_station_id_for_line_without_number");
        fields.push_back("min_price_for_autorouting");
        fields.push_back("our_numbers_id");
    }
};
