#pragma once

#include "../../src/common.h"
#include "../models/ConfigVersion.h"
#include "../lists/AirpList.h"
#include "../lists/NumberList.h"
#include "../lists/OutcomeList.h"
#include "../lists/PrefixlistList.h"
#include "../lists/PrefixlistPrefixList.h"
#include "../lists/RouteCaseList.h"
#include "../lists/ReleaseReasonList.h"
#include "../lists/RouteTableList.h"
#include "../lists/RouteTableRouteList.h"

class ConfigVersionData {
public:
    ConfigVersion * version;
    AirpList * airpList;
    NumberList * numberList;
    OutcomeList * outcomeList;
    PrefixlistList * prefixlistList;
    PrefixlistPrefixList * prefixlistPrefixList;
    RouteCaseList * routeCaseList;
    ReleaseReasonList * releaseReasonList;
    RouteTableList * routeTableList;
    RouteTableRouteList * routeTableRouteList;

    ConfigVersionData();

    static ConfigVersionData * load(BDb *db);
    virtual ~ConfigVersionData();
    int getId();
protected:
    bool ready();
};

