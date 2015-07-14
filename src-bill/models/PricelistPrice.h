#pragma once

#include "../common.h"

struct PricelistPrice {
    int pricelist_id;
    char prefix[20];
    time_t date_from;
    time_t date_to;
    double price;

    void dump(stringstream &trace) {
        trace << "(";
        trace << "pricelist_id: " << pricelist_id << ", ";
        trace << "prefix: " << prefix << ", ";
        trace << "date_from: " << string_time(date_from) << ", ";
        trace << "date_to: " << string_time(date_to) << ", ";
        trace << "price: " << price << ", ";
        trace << ")";
    }
};