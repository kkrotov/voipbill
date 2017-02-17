#pragma once

#include "../common.h"

struct ServiceNumber {
    int id;
    int server_id;
    int client_account_id;
    char did[20];
    int lines_count;
    time_t activation_dt;
    time_t expire_dt;
    char tech_number[20];
    int tech_number_operator_id;
    bool is_local;

    // Получить номер для использования в чёрном списке.
    const char* asBlacklistNumber() {
        return tech_number[0] ? tech_number : did;
    }

    bool is7800() {
        // Если номер начинается с 7800 и он не 4-значный:
        return 0 == strncmp("7800", did, 4) && did[4];
    }

    void dump(stringstream &trace) {
        trace << "(";
        trace << "id: " << id << ", ";
        trace << "server_id: " << server_id << ", ";
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
