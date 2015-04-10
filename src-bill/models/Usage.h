#pragma once

#include <time.h>

struct Usage {
    int id;
    int client_id;
    char phone_num[16];
    time_t actual_from;
    time_t actual_to;
    int no_of_lines;
    int region;
};
