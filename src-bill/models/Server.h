#pragma once

#include "../common.h"
#include <sys/socket.h>
#include <netinet/in.h>

struct Server {
    int id;
    int hub_id;
    int low_balance_outcome_id;
    int blocked_outcome_id;
    char calling_station_id_for_line_without_number[100];
    int min_price_for_autorouting;
    int our_numbers_id;
    int emergency_prefixlist_id;
    vector<int> call_sync_delay;
    vector<int> cdr_sync_delay;
    vector<int> call_save_delay;
    vector<int> cdr_proc_wait_count;
    vector<int> call_save_wait_count;
    vector<int> thread_error_count;
    vector<int> radius_request_delay;
    vector<string> service_numbers;
    vector<int> main_event_count;
    vector<int> local_event_count;
    string name;
    in_addr nas_ip_address;
};
