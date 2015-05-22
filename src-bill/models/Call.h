#pragma once

#include "Cdr.h"

#define CALL_ORIG               true
#define CALL_TERM               false

struct Call {
    long long int id;
    long long int peer_id;
    long long int cdr_id;
    time_t connect_time;
    char orig;

    int trunk_id;
    int account_id;
    int trunk_service_id;
    int number_service_id;

    long long int src_number;
    long long int dst_number;

    int billed_time;
    double rate;
    double cost;
    double tax_cost;
    double interconnect_rate;
    double interconnect_cost;

    int service_package_id;
    int service_package_limit_id;
    int package_time;
    double package_credit;

    int pricelist_id;
    long long int prefix;

    int destination_id;
    int geo_id;

    bool mob;
    int geo_operator_id;
    int operator_id;

    long long int cdr_call_id;


    void dump(stringstream &trace) {
        trace << "(";
        trace << "id " << id << ", ";
        trace << "peer_id: " << peer_id << ", ";
        trace << "cdr_id: " << cdr_id << ", ";
        trace << "connect_time: " << string_time(connect_time) << ", ";
        trace << "orig: " << orig << ", ";
        trace << "account_id: " << account_id << ", ";
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
        trace << "service_package_limit_id: " << service_package_limit_id << ", ";
        trace << "package_time: " << package_time << ", ";
        trace << "package_credit " << package_credit << ", ";
        trace << "pricelist_id: " << pricelist_id << ", ";
        trace << "prefix: " << prefix << ", ";
        trace << "destination_id: " << destination_id << ", ";
        trace << "geo_id: " << geo_id << ", ";
        trace << "mob: " << mob << ", ";
        trace << "geo_operator_id: " << geo_operator_id << ", ";
        trace << "operator_id: " << operator_id << ", ";
        trace << ")";
    }

    Call(Cdr * cdr, bool orig) {
        this->orig = orig;

        id = 0;
        peer_id = 0;
        cdr_id = cdr->id;
        connect_time = cdr->connect_time;

        account_id = 0;
        trunk_service_id = 0;
        number_service_id = 0;

        src_number = 0;
        dst_number = 0;

        billed_time = cdr->session_time;
        rate = 0.0;
        cost = 0.0;
        tax_cost = 0.0;
        interconnect_rate = 0.0;
        interconnect_cost = 0.0;

        service_package_id = 0;
        service_package_limit_id = 0;
        package_time = 0;
        package_credit = 0.0;

        pricelist_id = 0;
        prefix = 0;

        destination_id = 2;
        geo_id = 0;

        mob = false;
        geo_operator_id = 0;
        operator_id = 0;

        cdr_call_id = cdr->call_id;

        make_dt();
    }

    DT dt;
    void make_dt() {
        struct tm ttt;
        gmtime_r(&connect_time, &ttt);
        dt.day = connect_time - ttt.tm_hour * 3600 - ttt.tm_min * 60 - ttt.tm_sec;
        dt.month = dt.day - (ttt.tm_mday - 1)*86400;
    }

    bool isLocal() {
        return destination_id < 0;
    }

    bool isLocalOrZona() {
        return destination_id <= 0;
    }

    bool isZona() {
        return destination_id == 0;
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

    bool isSNG() {
        return destination_id == 3;
    }
};