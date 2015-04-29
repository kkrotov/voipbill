#pragma once

#include <time.h>

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
        trace << "date_from: " << date_from << ", ";
        trace << "date_to: " << date_to << ", ";
        trace << "price: " << price << ", ";
        trace << ")";
    }
};