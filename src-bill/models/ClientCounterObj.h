#pragma once

#include "../../src/common.h"

struct ClientCounterObj {
    int client_id;
    double sum;
    double sum_day;
    double sum_month;
    time_t amount_month;
    time_t amount_day;
    time_t amount_date;

    double sumDay(double tax_rate) {
        if (abs(amount_day - get_tday()) < 43200) {
            return sum_day * (1 + tax_rate);
        } else {
            return 0;
        }
    }

    double sumMonth(double tax_rate) {
        if (abs(amount_month - get_tmonth()) < 43200) {
            return sum_month * (1 + tax_rate);
        } else {
            return 0;
        }
    }

    double sumBalance(double tax_rate) {
        return sum * (1 + tax_rate);
    }
};