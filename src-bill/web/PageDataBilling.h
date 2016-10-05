#pragma once

#include "BasePage.h"
#include "../classes/Repository.h"

using namespace std;

class PageDataBilling : public BasePage {
public:
    bool canHandle(std::string &path) {
        return path == "/data-billing";
    }
    void render(std::stringstream &html, map<string, string> &parameters) {
        renderHeader(html);

        Repository repository;

        renderBillingData(html, repository.billingData);
    }

    static void renderBillingData(stringstream &html, DataBillingContainer *billingData) {
        html << "<table border=0>\n";
        html << "<tr>";
        html << "<td style='text-align: left' nowrap>cdrsWaitProcessing queue length:</td>";
        html << "<td style='text-align: left' nowrap>" << billingData->cdrsQueueSize() << "<br/>";
        html << "<td style='text-align: left' nowrap>callsWaitSaving queue length:</td>";
        html << "<td style='text-align: left' nowrap>" << billingData->callsQueueSize() << "<br/>";
        html << "</tr>\n";
        html << "<tr>";
        html << "<td style='text-align: left' nowrap>calced cdrs count:</td>";
        html << "<td style='text-align: left' nowrap>" << billingData->getCallsCounter() << "<br/>";
        html << "<td style='text-align: left' nowrap>saved calls count:</td>";
        html << "<td style='text-align: left' nowrap>" << billingData->getCallsStoredCounter() << "<br/>";
        html << "</tr>\n";
        html << "<tr>";
        html << "<td style='text-align: left' nowrap>last cdr id:</td>";
        html << "<td style='text-align: left' nowrap>" << billingData->getCdrsLastId() << "<br/>";
        html << "<td style='text-align: left' nowrap>last cdr time:</td>";
        html << "<td style='text-align: left' nowrap>" << string_time(billingData->getCdrsLastTime()) << "<br/>";
        html << "</tr>\n";
        html << "<tr>";
        html << "<td style='text-align: left' nowrap>last calc call id:</td>";
        html << "<td style='text-align: left' nowrap>" << billingData->getCallsLastId() << "<br/>";
        html << "<td style='text-align: left' nowrap>last calc call time:</td>";
        html << "<td style='text-align: left' nowrap>" << string_time(billingData->getCallsLastTime()) << "<br/>";
        html << "</tr>\n";
        html << "<tr>";
        html << "<td style='text-align: left' nowrap>last save call id:</td>";
        html << "<td style='text-align: left' nowrap>" << billingData->getCallsStoredLastId() << "<br/>";
        html << "<td style='text-align: left' nowrap>last save call time:</td>";
        html << "<td style='text-align: left' nowrap>" << string_time(billingData->getCallsStoredLastTime()) << "<br/>";
        html << "</tr>\n";
        html << "<tr>";
        html << "<td style='text-align: left' nowrap>last sync central call id:</td>";
        html << "<td style='text-align: left' nowrap>" << billingData->lastSyncCentralCallId << "<br/>";
        html << "<td style='text-align: left' nowrap>last sync central call time:</td>";
        html << "<td style='text-align: left' nowrap>" << string_time(billingData->lastSyncCentralCallTime) << "<br/>";
        html << "</tr>\n";
        html << "<tr>";
        html << "<td style='text-align: left' nowrap>last sync central cdr id:</td>";
        html << "<td style='text-align: left' nowrap>" << billingData->lastSyncCentralCdrId << "<br/>";
        html << "<td style='text-align: left' nowrap>last sync central cdr time:</td>";
        html << "<td style='text-align: left' nowrap>" << string_time(billingData->lastSyncCentralCdrTime) << "<br/>";
        html << "</tr>\n";
        html << "<tr>";
        html << "<td style='text-align: left' nowrap>last sync central cdr unfinished id:</td>";
        html << "<td style='text-align: left' nowrap>" << billingData->lastSyncCentralCdrUnfinishedId << "<br/>";
        html << "<td style='text-align: left' nowrap>last sync central cdr time:</td>";
        html << "<td style='text-align: left' nowrap>" << string_time(billingData->lastSyncCentralCdrUnfinishedTime) << "<br/>";
        html << "</tr>\n";
        html << "</table>\n";
        html << "</table>\n";
    }
};