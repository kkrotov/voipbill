#pragma once

#include "../common.h"
#include "Trunk.h"
#include "Pricelist.h"
#include "PricelistPrice.h"

struct ServiceTrunk {
    int id;
    int client_account_id;
    int trunk_id;
    time_t activation_dt;
    time_t expire_dt;
    bool orig_enabled;
    bool term_enabled;
    double orig_min_payment;
    double term_min_payment;

    void dump(stringstream &trace) {
        trace << "(";
        trace << "id: " << id << ", ";
        trace << "client_account_id: " << client_account_id << ", ";
        trace << "trunk_id: " << trunk_id << ", ";
        trace << "activation_dt: " << string_time(activation_dt) << ", ";
        trace << "expire_dt: " << string_time(expire_dt) << ", ";
        trace << "orig_enabled: " << orig_enabled << ", ";
        trace << "term_enabled: " << term_enabled << ", ";
        trace << "orig_min_payment: " << orig_min_payment << ", ";
        trace << "term_min_payment: " << term_min_payment << ", ";
        trace << ")";
    }
};

struct ServiceTrunkOrder {
    Trunk * trunk;
    ServiceTrunk * serviceTrunk;
    Pricelist * pricelist;
    PricelistPrice * price;
};

