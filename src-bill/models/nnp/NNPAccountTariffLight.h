#pragma once

#include "../../common.h"

struct NNPAccountTariffLight {
    int id;
    int service_number_id; // В базе поле называется account_tariff_id
    int account_client_id;
    int nnp_tariff_id;
    double coefficient;
    time_t activate_from, deactivate_from;

    int tarification_free_seconds;
    int tarification_interval_seconds;
    int tarification_type;
    int tarification_min_paid_seconds;
    
    double price;

    void dump(stringstream &trace) {
        trace << "(";
        trace << "id: " << id << ", ";
            trace << "service_number_id (account_tariff_id): " << service_number_id << ", ";
        trace << "account_client_id: " << account_client_id << ", ";
        trace << "nnp_tariff_id: " << nnp_tariff_id << ", ";
        trace << "activate_from: " << string_time(activate_from) << ", ";
        trace << "deactivate_from: " << string_time(deactivate_from) << ", ";
        trace << "coefficient: " << coefficient << ", ";
        trace << "tarification_free_seconds: " << tarification_free_seconds << ", ";
        trace << "tarification_interval_seconds: " << tarification_interval_seconds << ", ";
        trace << "tarification_type: " << tarification_type << ", ";
        trace << "tarification_min_paid_seconds: " << tarification_min_paid_seconds << ", ";
        trace << "price: " << price << ", ";       
        trace << ")";
    }
};