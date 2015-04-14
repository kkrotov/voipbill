#pragma once

#include "Cdr.h"

#define CALL_ORIG               true
#define CALL_TERM               false
#define SERVICE_TYPE_NONE       0
#define SERVICE_TYPE_TRUNK      1
#define SERVICE_TYPE_NUMBER     2

struct Call {
    long long int id;
    long long int cdr_id;
    char call_time[28];
    char orig;

    int client_account_id;
    int service_type;
    int service_id;

    char src_number[33];
    char dst_number[33];
    char redirect_number[33];

    int billed_time;
    double rate;
    double cost;
    double tax_cost;

    int service_package_id;
    int service_package_limit_id;
    int package_time;
    double package_credit;

    int pricelist_id;
    char prefix[20];

    int destination_id;
    int geo_id;

    int dest;
    bool mob;
    int geo_operator_id;

    int kill_call_reason;

    Call(Cdr * cdr, bool orig) {
        this->orig = orig;

        id = 0;
        cdr_id = cdr->id;
        strcpy(call_time, cdr->connect_time);

        client_account_id = 0;
        service_type = SERVICE_TYPE_NONE;
        service_id = 0;

        strcpy(src_number, cdr->src_number);
        strcpy(dst_number, cdr->dst_number);
        strcpy(redirect_number, cdr->redirect_number);

        billed_time = 0;
        rate = 0.0;
        cost = 0.0;
        tax_cost = 0.0;

        service_package_id = 0;
        service_package_limit_id = 0;
        package_time = 0;
        package_credit = 0.0;

        pricelist_id = 0;
        prefix[0] = 0;

        destination_id = 0;
        geo_id = 0;

        dest = 2;
        mob = false;
        geo_operator_id = 0;

        kill_call_reason = 0;

        make_dt();
    }

    DT dt;
    void make_dt() {
        parseDateTime((char*) &time, dt);
    }

    bool isLocal() {
        return dest < 0;
    }

    bool isLocalOrZona() {
        return dest <= 0;
    }

    bool isZona() {
        return dest == 0;
    }

    bool isZonaMob() {
        return dest == 0 && mob;
    }

    bool isZonaStd() {
        return dest == 0 && !mob;
    }

    bool isRussian() {
        return dest == 1;
    }

    bool isRussianMob() {
        return dest == 1 && mob;
    }

    bool isRussianStd() {
        return dest == 1 && !mob;
    }

    bool isInternational() {
        return dest == 2;
    }

    bool isSNG() {
        return dest == 3;
    }
};