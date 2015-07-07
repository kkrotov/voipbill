#pragma once

#include <time.h>

struct ServiceNumber {
    int id;
    int client_account_id;
    char did[20];
    int lines_count;
    time_t activation_dt;
    time_t expire_dt;

    void dump(stringstream &trace) {
        trace << "(";
        trace << "id: " << id << ", ";
        trace << "client_account_id: " << client_account_id << ", ";
        trace << "did: " << did << ", ";
        trace << "lines_count: " << lines_count << ", ";
        trace << "activation_dt: " << string_time(activation_dt) << ", ";
        trace << "expire_dt: " << string_time(expire_dt) << ", ";
        trace << ")";
    }
};