#pragma once

struct Tariff {
    int id;
    int freemin;
    bool freemin_for_number;
    int pricelist_id;
    bool paid_redirect;
    bool tariffication_by_minutes;
    bool tariffication_full_first_minute;
    bool tariffication_free_first_seconds;

    void dump(stringstream &trace) {
        trace << "(";
        trace << "id " << id << ", ";
        trace << "freemin: " << freemin << ", ";
        trace << "freemin_for_number: " << freemin_for_number << ", ";
        trace << "pricelist_id: " << pricelist_id << ", ";
        trace << "paid_redirect: " << paid_redirect << ", ";
        trace << "tariffication_by_minutes: " << tariffication_by_minutes << ", ";
        trace << "tariffication_full_first_minute: " << tariffication_full_first_minute << ", ";
        trace << "tariffication_free_first_seconds: " << tariffication_free_first_seconds << ", ";
        trace << ")";
    }
};
