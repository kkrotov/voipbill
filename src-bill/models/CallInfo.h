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
    }
};