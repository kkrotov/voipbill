#pragma once

#include <time.h>

struct TariffChangeLog {
    int usage_id;
    time_t activation_date;
    time_t expire_dt;
    int tariff_id_local;
    int tariff_id_local_mob;
    int tariff_id_russia;
    int tariff_id_russia_mob;
    int tariff_id_intern;
    int tariff_id_sng;


    void dump(stringstream &trace) {
        trace << "(";
        trace << "usage_id " << usage_id << ", ";
        trace << "activation_date: " << activation_date << ", ";
        trace << "expire_dt: " << expire_dt << ", ";
        trace << "tariff_id_local: " << tariff_id_local << ", ";
        trace << "tariff_id_local_mob: " << tariff_id_local_mob << ", ";
        trace << "tariff_id_russia: " << tariff_id_russia << ", ";
        trace << "tariff_id_russia_mob: " << tariff_id_russia_mob << ", ";
        trace << "tariff_id_intern: " << tariff_id_intern << ", ";
        trace << "tariff_id_sng: " << tariff_id_sng << ", ";
        trace << ")";
    }
};
