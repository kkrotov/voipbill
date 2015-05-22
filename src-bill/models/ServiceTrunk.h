#pragma once

#include <time.h>

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
    int operator_id;

    void dump(stringstream &trace) {
        trace << "(";
        trace << "id: " << id << ", ";
        trace << "client_account_id: " << client_account_id << ", ";
        trace << "trunk_id: " << trunk_id << ", ";
        trace << "activation_dt: " << activation_dt << ", ";
        trace << "expire_dt: " << expire_dt << ", ";
        trace << "orig_enabled: " << orig_enabled << ", ";
        trace << "term_enabled: " << term_enabled << ", ";
        trace << "orig_min_payment: " << orig_min_payment << ", ";
        trace << "term_min_payment: " << term_min_payment << ", ";
        trace << "operator_id: " << operator_id << ", ";
        trace << ")";
    }
};