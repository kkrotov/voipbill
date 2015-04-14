#pragma once

#include <time.h>

struct Usage {
    long long int phone_num;
    int id;
    int client_id;
    int no_of_lines;
    time_t actual_from;
    time_t actual_to;
};
