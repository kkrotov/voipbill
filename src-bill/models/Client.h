#pragma once

#include "../common.h"

struct Client {
    int id;
    int limit_d;
    int credit;
    double balance;
    bool disabled;
    time_t amount_date;
    int organization_id;
    bool price_include_vat;
    short timezone_offset;
    bool is_blocked;
    bool anti_froud_disabled;

    void dump(stringstream &trace) {
        trace << "(";
        trace << "id: " << id << ", ";
        trace << "limit_d: " << limit_d << ", ";
        trace << "credit: " << credit << ", ";
        trace << "balance: " << balance << ", ";
        trace << "disabled: " << disabled << ", ";
        trace << "amount_date: " << string_time(amount_date) << ", ";
        trace << "organization_id: " << organization_id << ", ";
        trace << "price_include_vat: " << price_include_vat << ", ";
        trace << "timezone_offset: " << timezone_offset << ", ";
        trace << "is_blocked: " << is_blocked << ", ";
        trace << "anti_froud_disabled: " << anti_froud_disabled << ", ";
        trace << ")";
    }

    bool hasCreditLimit() {
        return credit >= 0;
    }

    bool hasDailyLimit() {
        return limit_d != 0;
    }

    bool isConsumedCreditLimit(double value) {
        return hasCreditLimit() && (balance + credit + value < 0);
    }

    bool isConsumedDailyLimit(double value) {
        return hasDailyLimit() && (limit_d + value < 0);
    }

    double getPriceWithVat(double price, double vat_rate) {
        if (price_include_vat) {
            return price;
        }

        return price * (1 + vat_rate);
    }
};
