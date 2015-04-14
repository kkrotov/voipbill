#pragma once

#include <time.h>

struct PriceObj {
    int pricelist_id;
    char prefix[20];
    time_t date_from;
    time_t date_to;
    unsigned int price; // x4
};