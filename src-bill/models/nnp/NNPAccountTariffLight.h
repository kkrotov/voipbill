#pragma once

#include "../../common.h"

#define ACCOUNT_TARIFF_LIGHT_SERVICE_TYPE_SIMPLE_NUMBER  3

struct NNPAccountTariffLight {
    int id = 0;
    int service_number_id = 0; // В базе поле называется account_tariff_id
    int service_trunk_id = 0;
    int account_client_id = 0;
    int nnp_tariff_id = 0;
    double coefficient;
    time_t activate_from, deactivate_from;
    int service_type_id = 0;

    double price;

    void dump(stringstream &trace) {
        trace << "(";
        trace << "id: " << id << ", ";

        if (service_type_id == ACCOUNT_TARIFF_LIGHT_SERVICE_TYPE_SIMPLE_NUMBER) {
            trace << "service_number_id (account_tariff_id): " << service_number_id << ", ";
        } else {
            trace << "service_trunk_id (account_tariff_id): " << service_trunk_id << ", ";
        }

        trace << "account_client_id: " << account_client_id << ", ";
        trace << "nnp_tariff_id: " << nnp_tariff_id << ", ";
        trace << "activate_from: " << string_time(activate_from) << ", ";
        trace << "deactivate_from: " << string_time(deactivate_from) << ", ";
        trace << "coefficient: " << coefficient << ", ";
        trace << "price: " << price << ", ";
        trace << ")";
    }
};