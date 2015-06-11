#include "ThreadSync.h"

#include "../sync/PullClient.h"
#include "../sync/PullDefs.h"
#include "../sync/PullGeo.h"
#include "../sync/PullGeoPrefix.h"
#include "../sync/PullInstance.h"
#include "../sync/PullNetworkPrefix.h"
#include "../sync/PullOperator.h"
#include "../sync/PullPricelist.h"
#include "../sync/PullTariff.h"
#include "../sync/PullServiceNumber.h"
#include "../sync/PullServiceTrunk.h"
#include "../sync/PullServiceTrunkSettings.h"
#include "../sync/PullTariffLog.h"

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
#include "../sync/PullTrunkNumberPreprocessing.h"
#include "../sync/PullTrunkPriority.h"
#include "../sync/PullTrunkRule.h"

void ThreadSync::run() {

    manager->pull();

}

ThreadSync::ThreadSync() {
    id = idName();
    name = "Sync";
    
    manager = ManagerPull::instance();

    manager->add(new PullClient());
    manager->add(new PullGeo());
    manager->add(new PullGeoPrefix());
    manager->add(new PullInstance());
    manager->add(new PullNetworkPrefix());
    manager->add(new PullOperator());
    manager->add(new PullPricelist());
    manager->add(new PullTariff());
    manager->add(new PullDefs());
    manager->add(new PullServiceNumber());
    manager->add(new PullServiceTrunk());
    manager->add(new PullServiceTrunkSettings());
    manager->add(new PullLogTariff());

    manager->add(new PullServer());
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
    manager->add(new PullTrunkNumberPreprocessing());
    manager->add(new PullTrunkPriority());
    manager->add(new PullTrunkRule());
}
