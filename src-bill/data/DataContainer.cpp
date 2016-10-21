#include "DataContainer.h"

DataContainer * DataContainer::instance() {
    static DataContainer inst;
    return &inst;
}

void DataContainer::loadAll(BDb * db) {
    server.load(db);
    instanceSettings.load(db);
    airp.load(db);
    number.load(db);
    outcome.load(db);
    prefixlist.load(db);
    prefixlistPrefix.load(db);
    releaseReason.load(db);
    routeCase.load(db);
    routeTable.load(db);
    routeTableRoute.load(db);
    trunk.load(db);
    trunkGroup.load(db);
    trunkGroupItem.load(db);
    trunkByName.load(db);
    trunkByAlias.load(db);
    trunkNumberPreprocessing.load(db);
    trunkPriority.load(db);
    trunkRule.load(db);
    trunkTrunkRule.load(db);
    client.load(db);
    organization.load(db);
    voipOperator.load(db);
    pricelist.load(db);
    pricelistPrice.load(db);
    networkPrefix.load(db);
    geo.load(db);
    mobPrefix.load(db);
    geoPrefix.load(db);
    serviceNumber.load(db);
    serviceNumberByTechNumber.load(db);
    serviceNumberPackage.load(db);
    serviceTrunk.load(db);
    serviceTrunkPackage.load(db);
    serviceTrunkSettings.load(db);
    tariff.load(db);
    tariffPackage.load(db);
    tariffChangeLog.load(db);
    statDestinationPrefixlists.load(db);
    statPrefixlist.load(db);
    currencyRate.load(db);
    activeCounter.load(serviceNumber.get(), serviceTrunk.get());
    nnpDestination.load(db);
    nnpNumberRangePrefix.load(db);
    nnpPackage.load(db);
    nnpPackagePrice.load(db);
    nnpPrefix.load(db);
    nnpRegion.load(db);
    nnpNumberRange.load(db);
    nnpOperator.load(db);
    nnpPackageMinute.load(db);
    nnpPackagePricelist.load(db);
    nnpPrefixDestination.load(db);
    nnpAccountTariffLight.load(db);
}
