#pragma once

#include "../common.h"

struct CurrencyRate {
    char currency[4];
    time_t date;
    double rate;

    void dump(stringstream &trace) {
        trace << "(";
        trace << "currency: " << currency << ", ";
        trace << "date: " << string_time(date) << ", ";
        trace << "rate: " << rate << ", ";
        trace << ")";
    }
};
