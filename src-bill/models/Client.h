#pragma once

#include "../common.h"
#include "../classes/Log.h"

struct Client {
    int id;
    int limit_d;
    int limit_d_mn;
    int credit;
    double balance;
    bool disabled;
    time_t amount_date;
    int organization_id;
    bool price_include_vat;
    short timezone_offset;
    bool is_blocked;
    bool anti_fraud_disabled;
    bool is_trunk_client;
    bool is_num_client;
    int account_version;

    void dump(stringstream &trace) {
        trace << "(";
        trace << "id: " << id << ", ";
        trace << "limit_d: " << limit_d << ", ";
        trace << "limit_d_mn: " << limit_d_mn << ", ";
        trace << "credit: " << credit << ", ";
        trace << "balance: " << balance << ", ";
        trace << "disabled: " << disabled << ", ";
        trace << "amount_date: " << string_time(amount_date) << ", ";
        trace << "organization_id: " << organization_id << ", ";
        trace << "price_include_vat: " << price_include_vat << ", ";
        trace << "timezone_offset: " << timezone_offset << ", ";
        trace << "is_blocked: " << is_blocked << ", ";
        trace << "is_trunk_client: " << is_trunk_client << ", ";
        trace << "anti_fraud_disabled: " << anti_fraud_disabled << ", ";
        trace << "account_version: " << account_version << ", ";
        trace << ")";
    }

    bool hasCreditLimit() {
        return credit >= 0;
    }

    bool hasDailyLimit() {
        return limit_d > 0 && !is_trunk_client;
    }

    bool hasDailyMNLimit() {
        return limit_d_mn > 0 && !is_trunk_client;
    }

    bool isConsumedCreditLimit(double value) {
        return hasCreditLimit() && (balance + credit + value < 0.00001);
    }

    bool isConsumedDailyLimit(double value) {
        return !is_trunk_client && ( hasDailyLimit() && (limit_d + value < 0.00001) || limit_d <= 0 );
    }

    bool isConsumedDailyMNLimit(double value) {
        return !is_trunk_client && ( hasDailyMNLimit() && (limit_d_mn + value < 0.00001) || limit_d_mn <= 0 );
    }


    double getPriceWithVat(double price, double vat_rate) {
        if (price_include_vat) {
            return price;
        }

        return price * (1 + vat_rate);
    }
};
