#pragma once

#include "../common.h"

struct ServiceNumber {
    int id;
    int client_account_id;
    char did[20];
    int lines_count;
    time_t activation_dt;
    time_t expire_dt;
    char tech_number[20];
    int tech_number_operator_id;

    void dump(stringstream &trace) {
        trace << "(";
        trace << "id: " << id << ", ";
        trace << "client_account_id: " << client_account_id << ", ";
        trace << "did: " << did << ", ";
        trace << "lines_count: " << lines_count << ", ";
        trace << "activation_dt: " << string_time(activation_dt) << ", ";
        trace << "expire_dt: " << string_time(expire_dt) << ", ";
        trace << "activation_dt_raw: " << activation_dt << ", ";
        trace << "expire_dt_raw: " << expire_dt << ", ";
        trace << "tech_number: " << tech_number << ", ";
        trace << "tech_number_operator_id: " << tech_number_operator_id << ", ";
        trace << ")";
    }
};
