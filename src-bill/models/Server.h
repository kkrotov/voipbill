#pragma once

#include "../common.h"

struct Server {
    int id;
    int low_balance_outcome_id;
    int blocked_outcome_id;
    char calling_station_id_for_line_without_number[100];
    int min_price_for_autorouting;
    int our_numbers_id;
};
