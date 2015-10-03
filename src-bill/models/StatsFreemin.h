#pragma once

#include "../common.h"

struct StatsFreemin
{
    int id;
    int service_number_id;
    time_t month_dt;
    int used_seconds;
    double used_credit;
    long long int min_call_id;
    long long int max_call_id;
};
