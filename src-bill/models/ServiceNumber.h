#pragma once

#include <time.h>

struct ServiceNumber {
    int id;
    int client_account_id;
    char did[20];
    int lines_count;
    time_t activation_dt;
    time_t expire_dt;
};