#pragma once

#include "../common.h"

#include "Cdr.h"

#define CALL_ORIG               true
#define CALL_TERM               false

struct Call {
    long long int id;
    long long int peer_id;
    long long int cdr_id;
    time_t connect_time;
    bool orig;                      // Плечо вызова.
    bool our;

    int trunk_id;                   // Транк плеча вызова. таблица auth.trunk
    int account_id;                 // Лицевой счет плеча вызова
    int trunk_service_id;           // Услуга "Транк" для плеча вызова. таблица billing.service_trunk
    int number_service_id;          // Услуга "Номер" для плеча вызова. таблица billing.service_number

    long long int src_number;
    long long int dst_number;

    int billed_time;
    double rate;
    double cost;
    double tax_cost;
    double interconnect_rate;
    double interconnect_cost;

    int service_package_id;
    int service_package_stats_id;
    int package_time;
    double package_credit;

    int trunk_settings_stats_id;

    int  account_version;              // Номер версии расчета примененного к звонку, 4 - обычная, 5 - nnp-пакеты
    int  stats_nnp_package_minute_id;  // номер счетчика минут на nnp-пакете применнноного к этому звонку.

    int pricelist_id;
    long long int prefix;

    int destination_id;
    bool mob;                       // Флаг "Звонок на мобильный"
    int geo_id;
    bool geo_mob;
    int geo_operator_id;

    long long int cdr_call_id;
    int disconnect_cause;

    bool is_service_number;


    void dump(stringstream &trace) {

        trace << "(";
        trace << "id " << id << ", ";
        trace << "peer_id: " << peer_id << ", ";
        trace << "cdr_id: " << cdr_id << ", ";
        trace << "connect_time: " << string_time(connect_time) << ", ";
        trace << "orig: " << (orig ? "true" : "false") << ", ";
        trace << "our: " << (our ? "true" : "false") << ", ";
        trace << "account_id: " << account_id << ", ";
        trace << "trunk_id: " << trunk_id << ", ";
        trace << "trunk_service_id: " << trunk_service_id << ", ";
        trace << "number_service_id: " << number_service_id << ", ";
        trace << "src_number: " << src_number << ", ";
        trace << "dst_number: " << dst_number << ", ";
        trace << "billed_time: " << billed_time << ", ";
        trace << "rate: " << rate << ", ";
        trace << "cost: " << cost << ", ";
        trace << "tax_cost: " << tax_cost << ", ";
        trace << "interconnect_rate: " << interconnect_rate << ", ";
        trace << "interconnect_cost: " << interconnect_cost << ", ";
        trace << "service_package_id: " << service_package_id << ", ";
        trace << "service_package_stats_id: " << service_package_stats_id << ", ";
        trace << "package_time: " << package_time << ", ";
        trace << "package_credit: " << package_credit << ", ";
        trace << "trunk_settings_stats_id: " << trunk_settings_stats_id << ", ";
        trace << "pricelist_id: " << pricelist_id << ", ";
        trace << "prefix: " << prefix << ", ";
        trace << "destination_id: " << destination_id << ", ";
        trace << "mob: " << (mob ? "true" : "false") << ", ";
        trace << "geo_id: " << geo_id << ", ";
        trace << "geo_mob: " << (geo_mob ? "true" : "false") << ", ";
        trace << "geo_operator_id: " << geo_operator_id << ", ";
        trace << "is_service_number: " << (is_service_number ? "true" : "false") << ", ";
        trace << "disconnect_cause: " << disconnect_cause;
        trace << ")";
    }

    Call(Cdr * cdr, bool orig) {
        this->orig = orig;
        our = false;

        id = 0;
        peer_id = 0;
        cdr_id = cdr->id;
        connect_time = cdr->connect_time;

        account_id = 0;
        trunk_service_id = 0;
        number_service_id = 0;

        src_number = atoll(cdr->src_number);
        dst_number = atoll(cdr->dst_number);

        billed_time = cdr->session_time;
        rate = 0.0;
        cost = 0.0;
        tax_cost = 0.0;
        interconnect_rate = 0.0;
        interconnect_cost = 0.0;

        service_package_id = 0;
        service_package_stats_id = 0;
        package_time = 0;
        package_credit = 0.0;

        trunk_settings_stats_id = 0;

        pricelist_id = 0;
        prefix = 0;

        destination_id = 2;
        mob = false;
        geo_id = 0;
        geo_mob = false;
        geo_operator_id = 0;

        cdr_call_id = cdr->call_id;
        disconnect_cause = cdr->disconnect_cause;

        is_service_number = false;
    }

    bool isLocal() {
        return destination_id < 0;
    }

    bool isZonaMob() {
        return destination_id == 0 && mob;
    }

    bool isZonaStd() {
        return destination_id == 0 && !mob;
    }

    bool isRussian() {
        return destination_id == 1;
    }

    bool isRussianMob() {
        return destination_id == 1 && mob;
    }

    bool isRussianStd() {
        return destination_id == 1 && !mob;
    }

    bool isInternational() {
        return destination_id == 2;
    }

};
