#pragma once

#include "../common.h"
#include "../classes/Log.h"

struct Client {
    int id;
    int limit_d;
    int credit;
    int credit_term;
    double balance;
    bool disabled;
    time_t amount_date;
    int organization_id;
    bool price_include_vat;
    short timezone_offset;
    bool is_blocked;
    bool anti_fraud_disabled;

    void dump(stringstream &trace) {
        trace << "(";
        trace << "id: " << id << ", ";
        trace << "limit_d: " << limit_d << ", ";
        trace << "credit: " << credit << ", ";
        trace << "credit_term: " << credit_term << ", ";
        trace << "balance: " << balance << ", ";
        trace << "disabled: " << disabled << ", ";
        trace << "amount_date: " << string_time(amount_date) << ", ";
        trace << "organization_id: " << organization_id << ", ";
        trace << "price_include_vat: " << price_include_vat << ", ";
        trace << "timezone_offset: " << timezone_offset << ", ";
        trace << "is_blocked: " << is_blocked << ", ";
        trace << "anti_fraud_disabled: " << anti_fraud_disabled << ", ";
        trace << ")";
    }

    bool hasCreditLimit() {
        return credit >= 0;
    }

    bool hasTermCreditLimit() {
        return credit_term >= 0;
    }

    bool hasDailyLimit() {
        return limit_d != 0;
    }

    bool isConsumedCreditLimit(double value) {
        return hasCreditLimit() && (balance + credit + value < 0.00001);
    }

    bool isConsumedTermCreditLimit(double value) {
        if (!hasTermCreditLimit())
        {
             Log::debug("Consumer "+lexical_cast<string>(id)+" has no credit limit");
             return false;
        }
        if (balance + credit_term + value < 0.00001)
        {
            Log::debug("Consumer "+lexical_cast<string>(id)+": credit limit is over");
            return true;
        }
        Log::debug("Consumer "+lexical_cast<string>(id)+" has balance of "+ lexical_cast<string>(balance+credit_term+value));
        return false;
    }

    bool isConsumedDailyLimit(double value) {
        return hasDailyLimit() && (limit_d + value < 0.00001);
    }

    double getPriceWithVat(double price, double vat_rate) {
        if (price_include_vat) {
            return price;
        }

        return price * (1 + vat_rate);
    }
};
