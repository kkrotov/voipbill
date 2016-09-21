#pragma once

#include "../common.h"

struct StatsAccount {
    int account_id;
    // начисления по звонкам с момента последнего выставления счетов 
    // sum(calls_raw.calls_raw.cost) where connect_time>billing.clients.amount_date
    double sum;
    // -"- за текущий день
    double sum_day;
    double sum_mn_day; // Сколько наговорили MN траффика в день

    double sum_month; // удалить
    // текущий день
    time_t amount_day;
    time_t amount_month; // удалить
    // дата последнего выставления счетов
    time_t amount_date;


    double sumDay(double vat_rate, bool filter_by_date = true) {
        if (!filter_by_date || abs(amount_day - get_tday(time(nullptr))) < 43200) {
            return sum_day * (1 + vat_rate);
        } else {
            return 0;
        }
    }

    double sumMNDay(double vat_rate, bool filter_by_date = true) {
        if (!filter_by_date || abs(amount_day - get_tday(time(nullptr))) < 43200) {
            return sum_mn_day * (1 + vat_rate);
        } else {
            return 0;
        }
    }

    double sumMonth(double vat_rate, bool filter_by_date = true) {
        if (!filter_by_date || abs(amount_month - get_tmonth(time(nullptr))) < 43200) {
            return sum_month * (1 + vat_rate);
        } else {
            return 0;
        }
    }

    double sumBalance(double vat_rate) {
        return sum * (1 + vat_rate);
    }
};
