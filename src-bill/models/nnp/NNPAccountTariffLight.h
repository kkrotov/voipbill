#pragma once

#include "../../common.h"

struct NNPAccountTariffLight {
    int id;
    long long int number;
    int account_client_id;
    int nnp_tariff_id;
    time_t activate_from,deactivate_from;

    void dump(stringstream &trace) {
        trace << "(";
        trace << "id: " << id << ", ";
        trace << "number: " << number << ", ";
        trace << "account_client_id: " << account_client_id << ", ";
        trace << "nnp_tariff_id: " << nnp_tariff_id << ", ";
        trace << "activate_from: " << string_time(activate_from) << ", ";
        trace << "deactivate_from: " << string_time(deactivate_from) << ", ";
        trace << ")";
    }
};