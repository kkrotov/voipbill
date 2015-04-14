#pragma once

#include <time.h>

struct PricelistPrice {
    int pricelist_id;
    long long int prefix;
    time_t date_from;
    time_t date_to;
    double price;
};