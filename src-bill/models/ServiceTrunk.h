#pragma once

#include <time.h>

struct ServiceTrunk {
    int id;
    int client_account_id;
    char trunk_name[33];
    time_t activation_dt;
    time_t expire_dt;
    bool orig_enabled;
    bool term_enabled;
    double orig_min_payment;
    double term_min_payment;
};