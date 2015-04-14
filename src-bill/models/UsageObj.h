#pragma once

#include <time.h>

struct UsageObj {
    long long int phone_num;
    int id;
    int client_id;
    int free_seconds;
    bool paid_redirect;
    bool tariffication_by_minutes;
    bool tariffication_full_first_minute;
    bool tariffication_free_first_seconds;
    int pl_local_id;
    int pl_local_mob_id;
    int pl_russia_id;
    int pl_russia_mob_id;
    int pl_intern_id;
    int pl_sng_id;
    bool isConnectedOperator() {
        return phone_num >= 100 && phone_num < 1000;
    }
};
