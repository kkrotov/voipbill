#include "ThreadSync.h"
#include "../classes/AppBill.h"

#include "../sync/PullClient.h"
#include "../sync/PullDefs.h"
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

    manager.pull();

}

void ThreadSync::htmlfull(stringstream &html) {
    this->html(html);

    html << "Region: <b>" << app().conf.instance_id << "</b><br/>\n";
    html << "Errors count: <b>" << manager.errors_count << "</b><br/>\n";
    html << "<br/>\n";

    html << "<table width=100% border=1>\n";
    html << "<tr><td></td><th>Full reload</th><th>Updated</th><th>Deleted</th><th>Errors</th><th>Last time</th><th>Total time</th><tr>\n";
    for (auto it = manager.pulls.cbegin(); it != manager.pulls.cend(); ++it) {
        BasePull * pull = it->second;
        html << "<tr>" <<
                "<th>" << pull->event << "</th>" <<
                "<td>" << lexical_cast<string>(pull->pull_count_full) << "</td>" <<
                "<td>" << lexical_cast<string>(pull->pull_count_updated) << "</td>" <<
                "<td>" << lexical_cast<string>(pull->pull_count_deleted) << "</td>" <<
                "<td>" << lexical_cast<string>(pull->pull_count_errors) << "</td>" <<
                "<td>" << pull->timer.sloop() << "</td>" <<
                "<td>" << pull->timer.sfull() << "</td>" <<
                "</tr>\n";
    }
    html << "</table><br/>\n";
}

ThreadSync::ThreadSync() {
    id = idName();
    name = "Sync";

    manager.add(new PullClient());
    manager.add(new PullGeoPrefix());
    manager.add(new PullInstance());
    manager.add(new PullNetworkPrefix());
    manager.add(new PullOperator());
    manager.add(new PullPricelist());
    manager.add(new PullTariff());
    manager.add(new PullDefs());
    manager.add(new PullServiceNumber());
    manager.add(new PullServiceTrunk());
    manager.add(new PullServiceTrunkSettings());
    manager.add(new PullLogTariff());

    manager.add(new PullServer());
    manager.add(new PullAirp());
    manager.add(new PullNumber());
    manager.add(new PullOutcome());
    manager.add(new PullPrefixlist());
    manager.add(new PullPrefixlistPrefix());
    manager.add(new PullReleaseReason());
    manager.add(new PullRouteCase());
    manager.add(new PullRouteTable());
    manager.add(new PullRouteTableRoute());
    manager.add(new PullRoutingReportData());
    manager.add(new PullTrunk());
    manager.add(new PullTrunkNumberPreprocessing());
    manager.add(new PullTrunkPriority());
    manager.add(new PullTrunkRule());

}
