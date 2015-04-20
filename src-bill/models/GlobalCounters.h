#pragma once

struct GlobalCounters {
    int client_id;
    int sum;
    int sum_day;
    int sum_month;

    double sumDay() {
        return ((int) (sum_day * 1.18 + 0.5)) / 100.0;
    }

    double sumMonth() {
        return ((int) (sum_month * 1.18 + 0.5)) / 100.0;
    }

    double sumBalance() {
        return ((int) (sum * 1.18 + 0.5)) / 100.0;
    }

};