#pragma once

#include "../common.h"
#include "Trunk.h"
#include "ServiceTrunkSettings.h"
#include "Pricelist.h"
#include "PricelistPrice.h"
#include "Client.h"
#include "ServiceTrunkSettings.h"
#include "StatsTrunkSettings.h"

struct ServiceTrunk {
    int id;
    int client_account_id;
    int trunk_id;
    time_t activation_dt;
    time_t expire_dt;
    bool orig_enabled;
    bool term_enabled;
    double orig_min_payment;
    double term_min_payment;

    void dump(stringstream &trace) {
        trace << "(";
        trace << "id: " << id << ", ";
        trace << "client_account_id: " << client_account_id << ", ";
        trace << "trunk_id: " << trunk_id << ", ";
        trace << "activation_dt: " << string_time(activation_dt) << ", ";
        trace << "expire_dt: " << string_time(expire_dt) << ", ";
        trace << "orig_enabled: " << orig_enabled << ", ";
        trace << "term_enabled: " << term_enabled << ", ";
        trace << "orig_min_payment: " << orig_min_payment << ", ";
        trace << "term_min_payment: " << term_min_payment << ", ";
        trace << ")";
    }
};

struct ServiceTrunkOrder {
    Trunk * trunk;
    ServiceTrunk * serviceTrunk;
    ServiceTrunkSettings * trunkSettings;
    Pricelist * pricelist;
    PricelistPrice * price;
    Client * account;
    ServiceTrunkSettings * trunkSettings;
    StatsTrunkSettings * statsTrunkSettings;

    void dump(stringstream &trace) {
        trace << "(";
        if (trunk != nullptr) {
            trace << "trunk_id: " << trunk->id << ", ";
            trace << "trunk_name: " << trunk->name << ", ";
        }
        if (account != nullptr) {
            trace << "account_id: " << account->id << ", ";
        }
        if (serviceTrunk != nullptr) {
            trace << "service_trunk_id: " << serviceTrunk->id << ", ";
        }
        if (trunkSettings != nullptr) {
            trace << "trunk_settings_id: " << trunkSettings->id << ", ";
        }
        if (statsTrunkSettings != nullptr) {
            trace << "stats_trunk_settings_id: " << statsTrunkSettings->id << ", ";
        }
        if (pricelist != nullptr) {
            trace << "pricelist_id: " << pricelist->id << ", ";
        }
        if (price != nullptr) {
            trace << "prefix: " << price->prefix << ", ";
            trace << "price: " << price->price << ", ";
        }
        trace << ")";
    }
};
