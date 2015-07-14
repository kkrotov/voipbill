#pragma once

#include "../common.h"

struct Client {
    int id;
    int limit_m;
    int limit_d;
    int credit;
    double balance;
    bool disabled;
    time_t amount_date;
    time_t last_payed_month;
    int organization_id;
    bool price_include_vat;
    short timezone_offset;

    void dump(stringstream &trace) {
        trace << "(";
        trace << "id: " << id << ", ";
        trace << "limit_m: " << limit_m << ", ";
        trace << "limit_d: " << limit_d << ", ";
        trace << "credit: " << credit << ", ";
        trace << "balance: " << balance << ", ";
        trace << "disabled: " << disabled << ", ";
        trace << "amount_date: " << string_time(amount_date) << ", ";
        trace << "last_payed_month: " << string_time(last_payed_month) << ", ";
        trace << "organization_id: " << organization_id << ", ";
        trace << "price_include_vat: " << price_include_vat << ", ";
        trace << "timezone_offset: " << timezone_offset << ", ";
        trace << ")";
    }

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

    double getPriceWithVat(double price, double vat_rate) {
        if (price_include_vat) {
            return price;
        }

        return price * (1 + vat_rate);
    }
};
