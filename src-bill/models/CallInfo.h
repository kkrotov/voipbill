#pragma once

#include "../common.h"

#include "Call.h"
#include "Cdr.h"
#include "Trunk.h"
#include "Client.h"
#include "ServiceNumber.h"
#include "ServiceTrunk.h"
#include "ServicePackage.h"
#include "Tariff.h"
#include "TariffChangeLog.h"
#include "TariffPackage.h"
#include "Pricelist.h"
#include "PricelistPrice.h"
#include "Geo.h"

struct DT {
    time_t day;
    time_t month;
};

struct CallInfo {
    Call * call;
    Trunk * trunk;
    Client * account;
    ServiceNumber * serviceNumber;
    ServiceTrunk * serviceTrunk;
    ServicePackage *servicePackagePrepaid;
    ServicePackage * servicePackagePricelist;
    TariffChangeLog * logTariff;
    Tariff * mainTariff;
    Tariff * tariff;
    TariffPackage * tariffPackagePrepaid;
    Pricelist * pricelist;
    PricelistPrice * price;
    Geo * geo;

    DT dt;

    CallInfo() {
        call = nullptr;
        trunk = nullptr;
        account = nullptr;
        serviceNumber = nullptr;
        serviceTrunk = nullptr;
        servicePackagePrepaid = nullptr;
        servicePackagePricelist = nullptr;
        logTariff = nullptr;
        mainTariff = nullptr;
        tariffPackagePrepaid = nullptr;
        pricelist = nullptr;
        price = nullptr;
        geo = nullptr;
    }

    void make_dt() {
        struct tm ttt;
        time_t offset = 3600 * account->timezone_offset;
        time_t connect_time = call->connect_time + offset;
        gmtime_r(&connect_time, &ttt);
        dt.day = connect_time - offset - ttt.tm_hour * 3600 - ttt.tm_min * 60 - ttt.tm_sec;
        dt.month = dt.day - (ttt.tm_mday - 1)*86400;
    }
};