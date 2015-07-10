#pragma once

struct Pricelist {
    int id;
    int region;
    bool tariffication_by_minutes;
    bool tariffication_full_first_minute;
    bool orig;
    bool local;
    int local_network_config_id;
    double initiate_mgmn_cost;
    double initiate_zona_cost;
    bool price_include_vat;

    void dump(stringstream &trace) {
        trace << "(";
        trace << "id: " << id << ", ";
        trace << "region: " << region << ", ";
        trace << "tariffication_by_minutes: " << tariffication_by_minutes << ", ";
        trace << "tariffication_full_first_minute: " << tariffication_full_first_minute << ", ";
        trace << "orig: " << orig << ", ";
        trace << "local: " << local << ", ";
        trace << "local_network_config_id: " << local_network_config_id << ", ";
        trace << "initiate_mgmn_cost " << initiate_mgmn_cost << ", ";
        trace << "initiate_zona_cost: " << initiate_zona_cost << ", ";
        trace << "price_include_vat: " << price_include_vat << ", ";
        trace << ")";
    }
};