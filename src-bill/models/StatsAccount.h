#pragma once

#include "../common.h"

struct StatsAccount {
    int account_id;
    double sum;
    double sum_day;
    double sum_month;
    time_t amount_day;
    time_t amount_month;
    time_t amount_date;

    double sumDay(double vat_rate) {
        if (abs(amount_day - get_tday(time(nullptr))) < 43200) {
            return sum_day * (1 + vat_rate);
        } else {
            return 0;
        }
    }

    double sumMonth(double vat_rate) {
        if (abs(amount_month - get_tmonth(time(nullptr))) < 43200) {
            return sum_month * (1 + vat_rate);
        } else {
            return 0;
        }
    }

    double sumBalance(double vat_rate) {
        return sum * (1 + vat_rate);
    }
};
