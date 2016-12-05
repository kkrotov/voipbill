#include "PageHealthStatus.h"

bool PageHealthStatus::canHandle(std::string &path) {

    return path == "/health";
}

void PageHealthStatus::healthStatusHeader (stringstream &html) {

    html << "<tr>\n";
    html << "<td style='text-align: left' nowrap><a href='/test/healthcheck'><b>Parameter</b></td>\n";
    html << "<td style='text-align: left' nowrap><b>Current value</b></td>\n";
    html << "<td style='text-align: left' nowrap><b>Interval</b></td>\n";
    html << "<td style='text-align: left' nowrap><b>Status</b></td>\n";
    html << "<td style='text-align: left' nowrap><b>Message</b></td>\n";
    html << "</tr>";

    html << "<style>\n";
    html << ".ok { color: green; }\n";
    html << ".warn { color: #FFD700; }\n"; // Gold
    html << ".err { color: red; }\n";
    html << ".crit { background-color: red; color: black; }\n";
    html << ".unk { color: SteelBlue; }\n";
    html << ".tr_orig td { border-top: 1px solid silver; padding-top: 5px; }\n";
//    html << ".tr_term td { padding-bottom: 5px; }\n";
    html << "</style>\n";
}

void PageHealthStatus::healthStatusData (stringstream &html, SystemStatus statusData) {

    html << "<tr>\n";
    html << "<tr class='tr_orig'>\n";
    html << "<td style='text-align: left' nowrap><a href='/test/healthcheck?cmd="+statusData.itemId+"'>" << statusData.itemId << "</td>\n";
    html << "<td style='text-align: left' nowrap>" << statusData.itemValue << "</td>\n";
    std::string statusInterval="";
    if (statusData.prevValue.empty() && !statusData.nextValue.empty())
        statusInterval = "<= "+statusData.nextValue;

    if (!statusData.prevValue.empty() && statusData.nextValue.empty())
        statusInterval = "> "+statusData.prevValue;

    if (!statusData.prevValue.empty() && !statusData.nextValue.empty())
        statusInterval = "["+ statusData.prevValue + "," + statusData.nextValue+"]";

    html << "<td style='text-align: left' nowrap><b>" << statusInterval << "</b></td>\n";

    html << "<td ";
    std::string tclass="unk";
    if(statusData.statusId==HealthStatus::STATUS_OK)
        tclass = "ok";

    if(statusData.statusId==HealthStatus::STATUS_WARNING)
        tclass = "warn";

    if (statusData.statusId==HealthStatus::STATUS_ERROR)
        tclass = "err";

    if (statusData.statusId==HealthStatus::STATUS_CRITICAL)
        tclass = "crit";

    html << "<td class="+tclass+" style='text-align: left' nowrap><b>" << statusString[statusData.statusId] << "</b></td>\n";
    html << "<td style='text-align: left' nowrap>" << statusData.statusMessage << "</td>\n";
    html << "</tr>\n";
}

void PageHealthStatus::render(std::stringstream &html, map<string, string> &parameters) {

    renderHeader("health", html);

    std::vector<SystemStatus> systemStatus = app().healthCheckController.getStatus();
    if (systemStatus.empty())
        return;

    html << "<table width=100%>\n";
    healthStatusHeader(html);

    for (auto sysstat: systemStatus) {

        healthStatusData(html, sysstat);
    }
    html << "</table>\n";
}