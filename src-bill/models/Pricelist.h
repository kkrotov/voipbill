#pragma once

struct Pricelist {
    int id;
    int region;
    int operator_id;
    bool tariffication_by_minutes;
    bool tariffication_full_first_minute;
    bool orig;
    bool local;

    void dump(stringstream &trace) {
        trace << "(";
        trace << "id: " << id << ", ";
        trace << "region: " << region << ", ";
        trace << "operator_id: " << operator_id << ", ";
        trace << "tariffication_by_minutes: " << tariffication_by_minutes << ", ";
        trace << "tariffication_full_first_minute: " << tariffication_full_first_minute << ", ";
        trace << "orig: " << orig << ", ";
        trace << "local: " << local << ", ";
        trace << ")";
    }
};