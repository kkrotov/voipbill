#include "ThreadSync.h"

#include "../sync/PullClient.h"
#include "../sync/PullOrganization.h"
#include "../sync/PullDefs.h"
#include "../sync/PullGeo.h"
#include "../sync/PullMobPrefix.h"
#include "../sync/PullGeoPrefix.h"
#include "../sync/PullInstance.h"
#include "../sync/PullNetworkPrefix.h"
#include "../sync/PullOperator.h"
#include "../sync/PullPricelist.h"
#include "../sync/PullTariff.h"
#include "../sync/PullTariffPackage.h"
#include "../sync/PullServiceNumber.h"
#include "../sync/PullServicePackage.h"
#include "../sync/PullServiceTrunk.h"
#include "../sync/PullServiceTrunkSettings.h"
#include "../sync/PullTariffLog.h"
#include "../sync/PullStatPrefixlist.h"
#include "../sync/PullStatDestinationPrefixlists.h"
#include "../sync/PullCurrencyRate.h"

#include "../sync/PullServer.h"
#include "../sync/PullAirp.h"
#include "../sync/PullNumber.h"
#include "../sync/PullOutcome.h"
#include "../sync/PullPrefixlist.h"
#include "../sync/PullPrefixlistPrefix.h"
#include "../sync/PullReleaseReason.h"
#include "../sync/PullRouteCase.h"
#include "../sync/PullRouteTable.h"
#include "../sync/PullRouteTableRoute.h"
#include "../sync/PullRoutingReportData.h"
#include "../sync/PullTrunk.h"
#include "../sync/PullHub.h"

#include "../sync/PullTrunkGroup.h"
#include "../sync/PullTrunkGroupItem.h"
#include "../sync/PullTrunkNumberPreprocessing.h"
#include "../sync/PullTrunkPriority.h"
#include "../sync/PullTrunkRule.h"
#include "../sync/PullTrunkTrunkRule.h"

#include "../sync/nnp/NNPAccountTariffLightPull.h"
#include "../sync/nnp/NNPNumberRangePrefixPull.h"
#include "../sync/nnp/NNPOperatorPull.h"
#include "../sync/nnp/NNPPackagePricelistPull.h"
#include "../sync/nnp/NNPPackagePull.h"
#include "../sync/nnp/NNPPrefixPull.h"
#include "../sync/nnp/NNPDestinationPull.h"
#include "../sync/nnp/NNPNumberRangePull.h"
#include "../sync/nnp/NNPPackageMinutePull.h"
#include "../sync/nnp/NNPPackagePricePull.h"
#include "../sync/nnp/NNPPrefixDestinatioPull.h"
#include "../sync/nnp/NNPRegionPull.h"
#include "../sync/nnp/NNPCountryCodePull.h"

void ThreadSync::run() {

    manager->pull();

}

ThreadSync::ThreadSync() {
    id = idName();
    name = "Sync";
    
    manager = ManagerPull::instance();

    manager->add(new PullClient());
    manager->add(new PullOrganization());
    manager->add(new PullGeo());
    manager->add(new PullMobPrefix());
    manager->add(new PullGeoPrefix());
    manager->add(new PullInstance());
    manager->add(new PullNetworkPrefix());
    manager->add(new PullOperator());
    manager->add(new PullPricelist());
    manager->add(new PullTariff());
    manager->add(new PullTariffPackage());
    manager->add(new PullDefs());
    manager->add(new PullServiceNumber());
    manager->add(new PullServicePackage());
    manager->add(new PullServiceTrunk());
    manager->add(new PullServiceTrunkSettings());
    manager->add(new PullLogTariff());
    manager->add(new PullStatPrefixlist());
    manager->add(new PullStatDestinationPrefixlists());
    manager->add(new PullCurrencyRate());

    manager->add(new PullServer());
    manager->add(new PullHub());

    manager->add(new PullAirp());
    manager->add(new PullNumber());
    manager->add(new PullOutcome());
    manager->add(new PullPrefixlist());
    manager->add(new PullPrefixlistPrefix());
    manager->add(new PullReleaseReason());
    manager->add(new PullRouteCase());
    manager->add(new PullRouteTable());
    manager->add(new PullRouteTableRoute());
    manager->add(new PullRoutingReportData());
    manager->add(new PullTrunk());
    manager->add(new PullTrunkGroup());
    manager->add(new PullTrunkGroupItem());
    manager->add(new PullTrunkNumberPreprocessing());
    manager->add(new PullTrunkPriority());
    manager->add(new PullTrunkRule());
    manager->add(new PullTrunkTrunkRule());

    manager->add(new NNPAccountTariffLightPull());
    manager->add(new NNPNumberRangePrefixPull());
    manager->add(new NNPOperatorPull());
    manager->add(new NNPPackagePricelistPull());
    manager->add(new NNPPackagePull());
    manager->add(new NNPPrefixPull());
    manager->add(new NNPDestinationPull());
    manager->add(new NNPNumberRangePull());
    manager->add(new NNPPackageMinutePull());
    manager->add(new NNPPackagePricePull());
    manager->add(new NNPPrefixDestinatioPull());
    manager->add(new NNPRegionPull());
    manager->add(new NNPCountryCodePull());
}
