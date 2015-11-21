#pragma once

#include "../common.h"

struct StatsPackage {
    int id;
    int package_id;
    time_t activation_dt;
    time_t expire_dt;
    int used_seconds;
    double used_credit;
    int paid_seconds;
    long long int min_call_id;
    long long int max_call_id;
};
