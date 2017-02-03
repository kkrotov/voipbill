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
        fields.push_back("service_numbers");
        fields.push_back("hub_id");
        fields.push_back("emergency_prefixlist_id");
        fields.push_back("h_call_sync_delay");
        fields.push_back("h_cdr_sync_delay");
        fields.push_back("h_call_save_delay");
        fields.push_back("h_cdr_proc_wait_count");
        fields.push_back("h_call_save_wait_count");
        fields.push_back("h_thread_error_count");
        fields.push_back("h_radius_request_delay");
        fields.push_back("h_event_management");
        fields.push_back("h_local_events");
        fields.push_back("name");

        datatype.push_back("id smallint");
        datatype.push_back("low_balance_outcome_id integer");
        datatype.push_back("blocked_outcome_id integer");
        datatype.push_back("calling_station_id_for_line_without_number character varying(100)");
        datatype.push_back("min_price_for_autorouting integer");
        datatype.push_back("our_numbers_id integer");
        datatype.push_back("service_numbers text");
        datatype.push_back("hub_id integer");
        datatype.push_back("emergency_prefixlist_id integer");

        datatype.push_back("h_call_sync_delay varchar");
        datatype.push_back("h_cdr_sync_delay varchar");
        datatype.push_back("h_call_save_delay varchar");
        datatype.push_back("h_cdr_proc_wait_count varchar");
        datatype.push_back("h_call_save_wait_count varchar");
        datatype.push_back("h_thread_error_count varchar");
        datatype.push_back("h_radius_request_delay varchar");
        datatype.push_back("h_event_management varchar");
        datatype.push_back("h_local_events varchar");
        datatype.push_back("name varchar");
    }
};
