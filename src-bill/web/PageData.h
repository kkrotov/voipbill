#pragma once

#include "BasePage.h"
#include "../data/DataContainer.h"
#include "../data/DataBillingContainer.h"

using namespace std;

class PageData : public BasePage {
public:
    bool canHandle(std::string &path) {
        return path == "/data";
    }
    void render(std::stringstream &html, map<string, string> &parameters) {
        renderHeader(html);

        auto data = DataContainer::instance();
        auto billingData = DataBillingContainer::instance();

        html << "<table border=1 width=100%>";
        html << "<tr><th></th><th>Updated at</th><th>Size</th><th>Rows</th><th>Last time</th><th>Total time</th></tr>";
        {
            auto dl = &data->server;
            html << "<tr><th>server</th>";
            html << "<td>" << string_time(dl->time()) << "</td><td>" << dl->size() / 1024 << " Kb</td><td>" <<
            dl->rows() << "</td><td>" << dl->timer.sloop() << "</td><td>" << dl->timer.sfull() << "</td>";
            html << "</tr>\n";
        }
        {
            auto dl = &data->instanceSettings;
            html << "<tr><th>instanceSettings</th>";
            html << "<td>" << string_time(dl->time()) << "</td><td>" << dl->size() / 1024 << " Kb</td><td>" <<
            dl->rows() << "</td><td>" << dl->timer.sloop() << "</td><td>" << dl->timer.sfull() << "</td>";
            html << "</tr>\n";
        }
        {
            auto dl = &data->airp;
            html << "<tr><th>airp</th>";
            html << "<td>" << string_time(dl->time()) << "</td><td>" << dl->size() / 1024 << " Kb</td><td>" <<
            dl->rows() << "</td><td>" << dl->timer.sloop() << "</td><td>" << dl->timer.sfull() << "</td>";
            html << "</tr>\n";
        }
        {
            auto dl = &data->number;
            html << "<tr><th>number</th>";
            html << "<td>" << string_time(dl->time()) << "</td><td>" << dl->size() / 1024 << " Kb</td><td>" <<
            dl->rows() << "</td><td>" << dl->timer.sloop() << "</td><td>" << dl->timer.sfull() << "</td>";
            html << "</tr>\n";
        }
        {
            auto dl = &data->outcome;
            html << "<tr><th>outcome</th>";
            html << "<td>" << string_time(dl->time()) << "</td><td>" << dl->size() / 1024 << " Kb</td><td>" <<
            dl->rows() << "</td><td>" << dl->timer.sloop() << "</td><td>" << dl->timer.sfull() << "</td>";
            html << "</tr>\n";
        }
        {
            auto dl = &data->prefixlist;
            html << "<tr><th>prefixlist</th>";
            html << "<td>" << string_time(dl->time()) << "</td><td>" << dl->size() / 1024 << " Kb</td><td>" <<
            dl->rows() << "</td><td>" << dl->timer.sloop() << "</td><td>" << dl->timer.sfull() << "</td>";
            html << "</tr>\n";
        }
        {
            auto dl = &data->prefixlistPrefix;
            html << "<tr><th>prefixlistPrefix</th>";
            html << "<td>" << string_time(dl->time()) << "</td><td>" << dl->size() / 1024 << " Kb</td><td>" <<
            dl->rows() << "</td><td>" << dl->timer.sloop() << "</td><td>" << dl->timer.sfull() << "</td>";
            html << "</tr>\n";
        }
        {
            auto dl = &data->releaseReason;
            html << "<tr><th>releaseReason</th>";
            html << "<td>" << string_time(dl->time()) << "</td><td>" << dl->size() / 1024 << " Kb</td><td>" <<
            dl->rows() << "</td><td>" << dl->timer.sloop() << "</td><td>" << dl->timer.sfull() << "</td>";
            html << "</tr>\n";
        }
        {
            auto dl = &data->routeCase;
            html << "<tr><th>routeCase</th>";
            html << "<td>" << string_time(dl->time()) << "</td><td>" << dl->size() / 1024 << " Kb</td><td>" <<
            dl->rows() << "</td><td>" << dl->timer.sloop() << "</td><td>" << dl->timer.sfull() << "</td>";
            html << "</tr>\n";
        }
        {
            auto dl = &data->routeTable;
            html << "<tr><th>routeTable</th>";
            html << "<td>" << string_time(dl->time()) << "</td><td>" << dl->size() / 1024 << " Kb</td><td>" <<
            dl->rows() << "</td><td>" << dl->timer.sloop() << "</td><td>" << dl->timer.sfull() << "</td>";
            html << "</tr>\n";
        }
        {
            auto dl = &data->routeTableRoute;
            html << "<tr><th>routeTableRoute</th>";
            html << "<td>" << string_time(dl->time()) << "</td><td>" << dl->size() / 1024 << " Kb</td><td>" <<
            dl->rows() << "</td><td>" << dl->timer.sloop() << "</td><td>" << dl->timer.sfull() << "</td>";
            html << "</tr>\n";
        }
        {
            auto dl = &data->trunk;
            html << "<tr><th>trunk</th>";
            html << "<td>" << string_time(dl->time()) << "</td><td>" << dl->size() / 1024 << " Kb</td><td>" <<
            dl->rows() << "</td><td>" << dl->timer.sloop() << "</td><td>" << dl->timer.sfull() << "</td>";
            html << "</tr>\n";
        }
        {
            auto dl = &data->trunkNumberPreprocessing;
            html << "<tr><th>trunkNumberPreprocessing</th>";
            html << "<td>" << string_time(dl->time()) << "</td><td>" << dl->size() / 1024 << " Kb</td><td>" <<
            dl->rows() << "</td><td>" << dl->timer.sloop() << "</td><td>" << dl->timer.sfull() << "</td>";
            html << "</tr>\n";
        }
        {
            auto dl = &data->trunkPriority;
            html << "<tr><th>trunkPriority</th>";
            html << "<td>" << string_time(dl->time()) << "</td><td>" << dl->size() / 1024 << " Kb</td><td>" <<
            dl->rows() << "</td><td>" << dl->timer.sloop() << "</td><td>" << dl->timer.sfull() << "</td>";
            html << "</tr>\n";
        }
        {
            auto dl = &data->trunkRule;
            html << "<tr><th>trunkRule</th>";
            html << "<td>" << string_time(dl->time()) << "</td><td>" << dl->size() / 1024 << " Kb</td><td>" <<
            dl->rows() << "</td><td>" << dl->timer.sloop() << "</td><td>" << dl->timer.sfull() << "</td>";
            html << "</tr>\n";
        }
        {
            auto dl = &data->client;
            html << "<tr><th>client</th>";
            html << "<td>" << string_time(dl->time()) << "</td><td>" << dl->size() / 1024 << " Kb</td><td>" <<
            dl->rows() << "</td><td>" << dl->timer.sloop() << "</td><td>" << dl->timer.sfull() << "</td>";
            html << "</tr>\n";
        }
        {
            auto dl = &data->voipOperator;
            html << "<tr><th>voipOperator</th>";
            html << "<td>" << string_time(dl->time()) << "</td><td>" << dl->size() / 1024 << " Kb</td><td>" <<
            dl->rows() << "</td><td>" << dl->timer.sloop() << "</td><td>" << dl->timer.sfull() << "</td>";
            html << "</tr>\n";
        }
        {
            auto dl = &data->pricelist;
            html << "<tr><th>pricelist</th>";
            html << "<td>" << string_time(dl->time()) << "</td><td>" << dl->size() / 1024 << " Kb</td><td>" <<
            dl->rows() << "</td><td>" << dl->timer.sloop() << "</td><td>" << dl->timer.sfull() << "</td>";
            html << "</tr>\n";
        }
        {
            auto dl = &data->pricelistPrice;
            html << "<tr><th>pricelistPrice</th>";
            html << "<td>" << string_time(dl->time()) << "</td><td>" << dl->size() / 1024 << " Kb</td><td>" <<
            dl->rows() << "</td><td>" << dl->timer.sloop() << "</td><td>" << dl->timer.sfull() << "</td>";
            html << "</tr>\n";
        }
        {
            auto dl = &data->networkPrefix;
            html << "<tr><th>networkPrefix</th>";
            html << "<td>" << string_time(dl->time()) << "</td><td>" << dl->size() / 1024 << " Kb</td><td>" <<
            dl->rows() << "</td><td>" << dl->timer.sloop() << "</td><td>" << dl->timer.sfull() << "</td>";
            html << "</tr>\n";
        }
        {
            auto dl = &data->geoPrefix;
            html << "<tr><th>geoPrefix</th>";
            html << "<td>" << string_time(dl->time()) << "</td><td>" << dl->size() / 1024 << " Kb</td><td>" <<
            dl->rows() << "</td><td>" << dl->timer.sloop() << "</td><td>" << dl->timer.sfull() << "</td>";
            html << "</tr>\n";
        }
        {
            auto dl = &data->serviceNumber;
            html << "<tr><th>serviceNumber</th>";
            html << "<td>" << string_time(dl->time()) << "</td><td>" << dl->size() / 1024 << " Kb</td><td>" <<
            dl->rows() << "</td><td>" << dl->timer.sloop() << "</td><td>" << dl->timer.sfull() << "</td>";
            html << "</tr>\n";
        }
        {
            auto dl = &data->serviceTrunk;
            html << "<tr><th>serviceTrunk</th>";
            html << "<td>" << string_time(dl->time()) << "</td><td>" << dl->size() / 1024 << " Kb</td><td>" <<
            dl->rows() << "</td><td>" << dl->timer.sloop() << "</td><td>" << dl->timer.sfull() << "</td>";
            html << "</tr>\n";
        }
        {
            auto dl = &data->serviceTrunkSettings;
            html << "<tr><th>serviceTrunkSettings</th>";
            html << "<td>" << string_time(dl->time()) << "</td><td>" << dl->size() / 1024 << " Kb</td><td>" <<
            dl->rows() << "</td><td>" << dl->timer.sloop() << "</td><td>" << dl->timer.sfull() << "</td>";
            html << "</tr>\n";
        }
        {
            auto dl = &data->tariff;
            html << "<tr><th>tariff</th>";
            html << "<td>" << string_time(dl->time()) << "</td><td>" << dl->size() / 1024 << " Kb</td><td>" <<
            dl->rows() << "</td><td>" << dl->timer.sloop() << "</td><td>" << dl->timer.sfull() << "</td>";
            html << "</tr>\n";
        }
        {
            auto dl = &data->tariffChangeLog;
            html << "<tr><th>tariffChangeLog</th>";
            html << "<td>" << string_time(dl->time()) << "</td><td>" << dl->size() / 1024 << " Kb</td><td>" <<
            dl->rows() << "</td><td>" << dl->timer.sloop() << "</td><td>" << dl->timer.sfull() << "</td>";
            html << "</tr>\n";
        }
        {
            auto dl = &data->globalCounters;
            html << "<tr><th>globalCounters</th>";
            html << "<td>" << string_time(dl->time()) << "</td><td>" << dl->size() / 1024 << " Kb</td><td>" <<
            dl->rows() << "</td><td>" << dl->timer.sloop() << "</td><td>" << dl->timer.sfull() << "</td>";
            html << "</tr>\n";
        }
        {
            auto dl = &data->activeCounter;
            html << "<tr><th>Active counter</th>";
            if (dl != 0) {
                html << "<td>" << string_time(dl->time()) << "</td><td></td><td>" <<
                dl->rows() << "</td><td>" << dl->timer.sloop() << "</td><td>" <<
                dl->timer.sfull() << "</td>";
            } else {
                html << "<td colspan=5></td>";
            }
            html << "</tr>\n";
        }
        {
            auto dl = &billingData->clientCounter;
            html << "<tr><th>Client Counter</th>";
            if (dl != 0) {
                html << "<td>" << string_time(dl->time()) << "</td><td></td><td>" <<
                dl->rows() << "</td><td>" << dl->timer.sloop() << "</td><td>" <<
                dl->timer.sfull() << "</td>";
            } else {
                html << "<td colspan=5></td>";
            }
            html << "</tr>\n";
        }
        {
            auto dl = &billingData->fminCounter;
            html << "<tr><th>Fmin Counter</th>";
            if (dl != 0) {
                html << "<td>" << string_time(dl->time()) << "</td><td></td><td>" <<
                dl->rows() << "</td><td>" << dl->timer.sloop() << "</td><td>" << dl->timer.sfull() <<
                "</td>";
            } else {
                html << "<td colspan=5></td>";
            }
            html << "</tr>\n";
        }
        {
            auto dl = &billingData->clientLock;
            html << "<tr><th>Client Lock</th>";
            if (dl != 0) {
                html << "<td>" << string_time(dl->time()) << "</td><td></td><td>" <<
                dl->rows() << "</td><td>" << dl->timer.sloop() << "</td><td>" << dl->timer.sfull() <<
                "</td>";
            } else {
                html << "<td colspan=5></td>";
            }
            html << "</tr>\n";
        }

        html << "</table>\n";

    }
};