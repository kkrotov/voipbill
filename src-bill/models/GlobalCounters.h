#pragma once

struct GlobalCounters {
    int client_id;
    double sum;
    double sum_day;
    double sum_month;

    double sumDay(double tax_rate) {
        return sum_day * (1 + tax_rate);
    }

    double sumMonth(double tax_rate) {
        return sum_month * (1 + tax_rate);
    }

    double sumBalance(double tax_rate) {
        return sum *  (1 + tax_rate);
    }

};