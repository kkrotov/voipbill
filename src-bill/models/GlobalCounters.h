#pragma once

#include "../common.h"

struct GlobalCounters {
    int client_id;
    double sum;
    double sum_day;
    double sum_month;

    double sumDay(double vat_rate) {
        return sum_day * (1 + vat_rate);
    }

    double sumMonth(double vat_rate) {
        return sum_month * (1 + vat_rate);
    }

    double sumBalance(double vat_rate) {
        return sum *  (1 + vat_rate);
    }

};