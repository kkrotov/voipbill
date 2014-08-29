#include <memory>

#include "ConfigVersionData.h"
#include "../dao/ConfigVersionDao.h"
#include "../../src/classes/Log.h"

ConfigVersionData::ConfigVersionData() {
    version = 0;
    airpList = 0;
    numberList = 0;
    outcomeList = 0;
    prefixlistList = 0;
    prefixlistPrefixList = 0;
    routeCaseList = 0;
    releaseReasonList = 0;
    routeTableList = 0;
    routeTableRouteList = 0;
    operatorList = 0;
    routingReportDataList = 0;
}

ConfigVersionData::~ConfigVersionData() {
    if (version != 0) delete version;
    if (airpList != 0) delete airpList;
    if (numberList != 0) delete numberList;
    if (outcomeList != 0) delete outcomeList;
    if (prefixlistList != 0) delete prefixlistList;
    if (prefixlistPrefixList != 0) delete prefixlistPrefixList;
    if (routeCaseList != 0) delete routeCaseList;
    if (releaseReasonList != 0) delete releaseReasonList;
    if (routeTableList != 0) delete routeTableList;
    if (routeTableRouteList != 0) delete routeTableRouteList;
    if (operatorList != 0) delete operatorList;
    if (routingReportDataList != 0) delete routingReportDataList;
}

ConfigVersionData * ConfigVersionData::load(BDb *db) {

    auto data = new ConfigVersionData();

    try {
        ConfigVersionDao versionDao(db);

        data->version = versionDao.getCurrentVersion();

        data->airpList = new AirpList();
        data->airpList->load(db);

        data->numberList = new NumberList();
        data->numberList->load(db);

        data->outcomeList = new OutcomeList();
        data->outcomeList->load(db);

        data->prefixlistList = new PrefixlistList();
        data->prefixlistList->load(db);

        data->prefixlistPrefixList = new PrefixlistPrefixList();
        data->prefixlistPrefixList->load(db);

        data->routeCaseList = new RouteCaseList();
        data->routeCaseList->load(db);

        data->releaseReasonList = new ReleaseReasonList();
        data->releaseReasonList->load(db);

        data->routeTableList = new RouteTableList();
        data->routeTableList->load(db);

        data->routeTableRouteList = new RouteTableRouteList();
        data->routeTableRouteList->load(db);

        data->operatorList = new OperatorList();
        data->operatorList->load(db);

        data->routingReportDataList = new RoutingReportDataList();
        data->routingReportDataList->load(db);

    } catch (Exception &e) {
        delete data;
        Log::error(e.what());
        return 0;
    }


    if (data->ready()) {
        return data;
    } else {
        delete data;
    }
}

bool ConfigVersionData::ready() {
    if (version == 0) return false;
    return true;
}

int ConfigVersionData::getId() {
    return version->id;
}
