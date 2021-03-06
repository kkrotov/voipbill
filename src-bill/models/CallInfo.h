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
#include "Server.h"
#include "InstanceSettings.h"
#include "nnp/NNPNumberRange.h"
#include "nnp/NNPPackagePrice.h"
#include "nnp/NNPPackagePricelist.h"
#include "nnp/NNPPackage.h"
#include "nnp/NNPPackageMinute.h"

#include "ServiceTrunkSettings.h"

struct DT {
    time_t day;
    time_t month;
};

struct CallInfo {
    Call * call;
    Server *server;
    InstanceSettings *instanceSettings;
    Trunk * trunk;
    Client * account;
    ServiceNumber * serviceNumber;
    ServiceTrunk * serviceTrunk;
    ServicePackage *servicePackagePrepaid;
    ServicePackage * servicePackagePricelist;
    ServiceTrunkSettings * trunkSettings;
    TariffChangeLog * logTariff;
    Tariff * mainTariff;
    Tariff * tariff;
    TariffPackage * tariffPackagePrepaid;
    Pricelist * pricelist;
    PricelistPrice * price;
    Geo * geo;
    NNPNumberRange *nnpNumberRange;

    NNPPackage * nnpPackage = nullptr;
    NNPPackagePrice * nnpPackagePrice = nullptr;
    NNPPackagePricelist * nnpPackagePricelist = nullptr;

    bool callIncomingFromMegatrunk = false;

    DT dt;
    DT dtUtc;

    CallInfo() {
        call = nullptr;
        instanceSettings = nullptr;
        server = nullptr;
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
        nnpNumberRange = nullptr;
        nnpPackage = nullptr;
        nnpPackagePrice = nullptr;
        nnpPackagePricelist = nullptr;

    }

    void make_dt() {
        dt.day = get_tday(call->connect_time, account->timezone_offset);
        dt.month = get_tmonth(call->connect_time, account->timezone_offset);
        dtUtc.day = get_tday(call->connect_time);
        dtUtc.month = get_tmonth(call->connect_time);
    }
};