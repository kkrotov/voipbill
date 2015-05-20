#pragma once

struct GlobalCounters {
    int client_id;
    double sum;
    double sum_day;
    double sum_month;

    double sumDay() {
        return sum_day * 1.18;
    }

    double sumMonth() {
        return sum_month * 1.18;
    }

    double sumBalance() {
        return sum * 1.18;
    }

};