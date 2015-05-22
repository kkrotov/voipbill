#pragma once

struct Client {
    int id;
    int limit_m;
    int limit_d;
    int credit;
    double balance;
    bool disabled;
    time_t amount_date;
    time_t last_payed_month;

    bool hasCreditLimit() {
        return credit >= 0;
    }

    bool hasDailyLimit() {
        return limit_d != 0;
    }

    bool hasMonthlyLimit() {
        return limit_m != 0;
    }

    bool isConsumedCreditLimit(double value) {
        return hasCreditLimit() && (balance + credit + value < 0);
    }

    bool isConsumedDailyLimit(double value) {
        return hasDailyLimit() && (limit_d + value < 0);
    }

    bool isConsumedMonthlyLimit(double value) {
        return hasMonthlyLimit() && (limit_m + value < 0);
    }

};