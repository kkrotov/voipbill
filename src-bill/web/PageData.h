#pragma once

#include "BasePage.h"
#include "../classes/Repository.h"

using namespace std;

class PageData : public BasePage {
public:
    bool canHandle(std::string &path) {
        return path == "/data";
    }
    void render(std::stringstream &html, map<string, string> &parameters) {
        renderHeader(html);

        Repository repository;

        html << "<table border=1 width=100%>";
        html << "<tr><th></th><th>Updated at</th><th>Size</th><th>Rows</th><th>Last time</th><th>Total time</th></tr>";
        {
            auto dl = &repository.data->server;
            html << "<tr><th>server</th>";
            html << "<td>" << string_time(dl->time()) << "</td><td>" << dl->size() / 1024 << " Kb</td><td>" <<
            dl->rows() << "</td><td>" << dl->timer.sloop() << "</td><td>" << dl->timer.sfull() << "</td>";
            html << "</tr>\n";
        }
        {
            auto dl = &repository.data->instanceSettings;
            html << "<tr><th>instanceSettings</th>";
            html << "<td>" << string_time(dl->time()) << "</td><td>" << dl->size() / 1024 << " Kb</td><td>" <<
            dl->rows() << "</td><td>" << dl->timer.sloop() << "</td><td>" << dl->timer.sfull() << "</td>";
            html << "</tr>\n";
        }
        {
            auto dl = &repository.data->airp;
            html << "<tr><th>airp</th>";
            html << "<td>" << string_time(dl->time()) << "</td><td>" << dl->size() / 1024 << " Kb</td><td>" <<
            dl->rows() << "</td><td>" << dl->timer.sloop() << "</td><td>" << dl->timer.sfull() << "</td>";
            html << "</tr>\n";
        }
        {
            auto dl = &repository.data->number;
            html << "<tr><th>number</th>";
            html << "<td>" << string_time(dl->time()) << "</td><td>" << dl->size() / 1024 << " Kb</td><td>" <<
            dl->rows() << "</td><td>" << dl->timer.sloop() << "</td><td>" << dl->timer.sfull() << "</td>";
            html << "</tr>\n";
        }
        {
            auto dl = &repository.data->outcome;
            html << "<tr><th>outcome</th>";
            html << "<td>" << string_time(dl->time()) << "</td><td>" << dl->size() / 1024 << " Kb</td><td>" <<
            dl->rows() << "</td><td>" << dl->timer.sloop() << "</td><td>" << dl->timer.sfull() << "</td>";
            html << "</tr>\n";
        }
        {
            auto dl = &repository.data->prefixlist;
            html << "<tr><th>prefixlist</th>";
            html << "<td>" << string_time(dl->time()) << "</td><td>" << dl->size() / 1024 << " Kb</td><td>" <<
            dl->rows() << "</td><td>" << dl->timer.sloop() << "</td><td>" << dl->timer.sfull() << "</td>";
            html << "</tr>\n";
        }
        {
            auto dl = &repository.data->prefixlistPrefix;
            html << "<tr><th>prefixlistPrefix</th>";
            html << "<td>" << string_time(dl->time()) << "</td><td>" << dl->size() / 1024 << " Kb</td><td>" <<
            dl->rows() << "</td><td>" << dl->timer.sloop() << "</td><td>" << dl->timer.sfull() << "</td>";
            html << "</tr>\n";
        }
        {
            auto dl = &repository.data->releaseReason;
            html << "<tr><th>releaseReason</th>";
            html << "<td>" << string_time(dl->time()) << "</td><td>" << dl->size() / 1024 << " Kb</td><td>" <<
            dl->rows() << "</td><td>" << dl->timer.sloop() << "</td><td>" << dl->timer.sfull() << "</td>";
            html << "</tr>\n";
        }
        {
            auto dl = &repository.data->routeCase;
            html << "<tr><th>routeCase</th>";
            html << "<td>" << string_time(dl->time()) << "</td><td>" << dl->size() / 1024 << " Kb</td><td>" <<
            dl->rows() << "</td><td>" << dl->timer.sloop() << "</td><td>" << dl->timer.sfull() << "</td>";
            html << "</tr>\n";
        }
        {
            auto dl = &repository.data->routeTable;
            html << "<tr><th>routeTable</th>";
            html << "<td>" << string_time(dl->time()) << "</td><td>" << dl->size() / 1024 << " Kb</td><td>" <<
            dl->rows() << "</td><td>" << dl->timer.sloop() << "</td><td>" << dl->timer.sfull() << "</td>";
            html << "</tr>\n";
        }
        {
            auto dl = &repository.data->routeTableRoute;
            html << "<tr><th>routeTableRoute</th>";
            html << "<td>" << string_time(dl->time()) << "</td><td>" << dl->size() / 1024 << " Kb</td><td>" <<
            dl->rows() << "</td><td>" << dl->timer.sloop() << "</td><td>" << dl->timer.sfull() << "</td>";
            html << "</tr>\n";
        }
        {
            auto dl = &repository.data->trunk;
            html << "<tr><th>trunk</th>";
            html << "<td>" << string_time(dl->time()) << "</td><td>" << dl->size() / 1024 << " Kb</td><td>" <<
            dl->rows() << "</td><td>" << dl->timer.sloop() << "</td><td>" << dl->timer.sfull() << "</td>";
            html << "</tr>\n";
        }
        {
            auto dl = &repository.data->trunkGroup;
            html << "<tr><th>trunkGroup</th>";
            html << "<td>" << string_time(dl->time()) << "</td><td>" << dl->size() / 1024 << " Kb</td><td>" <<
            dl->rows() << "</td><td>" << dl->timer.sloop() << "</td><td>" << dl->timer.sfull() << "</td>";
            html << "</tr>\n";
        }
        {
            auto dl = &repository.data->trunkGroupItem;
            html << "<tr><th>trunkGroupItem</th>";
            html << "<td>" << string_time(dl->time()) << "</td><td>" << dl->size() / 1024 << " Kb</td><td>" <<
            dl->rows() << "</td><td>" << dl->timer.sloop() << "</td><td>" << dl->timer.sfull() << "</td>";
            html << "</tr>\n";
        }
        {
            auto dl = &repository.data->trunkNumberPreprocessing;
            html << "<tr><th>trunkNumberPreprocessing</th>";
            html << "<td>" << string_time(dl->time()) << "</td><td>" << dl->size() / 1024 << " Kb</td><td>" <<
            dl->rows() << "</td><td>" << dl->timer.sloop() << "</td><td>" << dl->timer.sfull() << "</td>";
            html << "</tr>\n";
        }
        {
            auto dl = &repository.data->trunkPriority;
            html << "<tr><th>trunkPriority</th>";
            html << "<td>" << string_time(dl->time()) << "</td><td>" << dl->size() / 1024 << " Kb</td><td>" <<
            dl->rows() << "</td><td>" << dl->timer.sloop() << "</td><td>" << dl->timer.sfull() << "</td>";
            html << "</tr>\n";
        }
        {
            auto dl = &repository.data->trunkRule;
            html << "<tr><th>trunkRule</th>";
            html << "<td>" << string_time(dl->time()) << "</td><td>" << dl->size() / 1024 << " Kb</td><td>" <<
            dl->rows() << "</td><td>" << dl->timer.sloop() << "</td><td>" << dl->timer.sfull() << "</td>";
            html << "</tr>\n";
        }
        {
            auto dl = &repository.data->trunkTrunkRule;
            html << "<tr><th>trunkTrunkRule</th>";
            html << "<td>" << string_time(dl->time()) << "</td><td>" << dl->size() / 1024 << " Kb</td><td>" <<
            dl->rows() << "</td><td>" << dl->timer.sloop() << "</td><td>" << dl->timer.sfull() << "</td>";
            html << "</tr>\n";
        }
        {
            auto dl = &repository.data->client;
            html << "<tr><th>client</th>";
            html << "<td>" << string_time(dl->time()) << "</td><td>" << dl->size() / 1024 << " Kb</td><td>" <<
            dl->rows() << "</td><td>" << dl->timer.sloop() << "</td><td>" << dl->timer.sfull() << "</td>";
            html << "</tr>\n";
        }
        {
            auto dl = &repository.data->organization;
            html << "<tr><th>organization</th>";
            html << "<td>" << string_time(dl->time()) << "</td><td>" << dl->size() / 1024 << " Kb</td><td>" <<
            dl->rows() << "</td><td>" << dl->timer.sloop() << "</td><td>" << dl->timer.sfull() << "</td>";
            html << "</tr>\n";
        }
        {
            auto dl = &repository.data->voipOperator;
            html << "<tr><th>voipOperator</th>";
            html << "<td>" << string_time(dl->time()) << "</td><td>" << dl->size() / 1024 << " Kb</td><td>" <<
            dl->rows() << "</td><td>" << dl->timer.sloop() << "</td><td>" << dl->timer.sfull() << "</td>";
            html << "</tr>\n";
        }
        {
            auto dl = &repository.data->pricelist;
            html << "<tr><th>pricelist</th>";
            html << "<td>" << string_time(dl->time()) << "</td><td>" << dl->size() / 1024 << " Kb</td><td>" <<
            dl->rows() << "</td><td>" << dl->timer.sloop() << "</td><td>" << dl->timer.sfull() << "</td>";
            html << "</tr>\n";
        }
        {
            auto dl = &repository.data->pricelistPrice;
            html << "<tr><th>pricelistPrice</th>";
            html << "<td>" << string_time(dl->time()) << "</td><td>" << dl->size() / 1024 << " Kb</td><td>" <<
            dl->rows() << "</td><td>" << dl->timer.sloop() << "</td><td>" << dl->timer.sfull() << "</td>";
            html << "</tr>\n";
        }
        {
            auto dl = &repository.data->networkPrefix;
            html << "<tr><th>networkPrefix</th>";
            html << "<td>" << string_time(dl->time()) << "</td><td>" << dl->size() / 1024 << " Kb</td><td>" <<
            dl->rows() << "</td><td>" << dl->timer.sloop() << "</td><td>" << dl->timer.sfull() << "</td>";
            html << "</tr>\n";
        }
        {
            auto dl = &repository.data->geo;
            html << "<tr><th>geo</th>";
            html << "<td>" << string_time(dl->time()) << "</td><td>" << dl->size() / 1024 << " Kb</td><td>" <<
            dl->rows() << "</td><td>" << dl->timer.sloop() << "</td><td>" << dl->timer.sfull() << "</td>";
            html << "</tr>\n";
        }
        {
            auto dl = &repository.data->geoPrefix;
            html << "<tr><th>geoPrefix</th>";
            html << "<td>" << string_time(dl->time()) << "</td><td>" << dl->size() / 1024 << " Kb</td><td>" <<
            dl->rows() << "</td><td>" << dl->timer.sloop() << "</td><td>" << dl->timer.sfull() << "</td>";
            html << "</tr>\n";
        }
        {
            auto dl = &repository.data->mobPrefix;
            html << "<tr><th>mobPrefix</th>";
            html << "<td>" << string_time(dl->time()) << "</td><td>" << dl->size() / 1024 << " Kb</td><td>" <<
            dl->rows() << "</td><td>" << dl->timer.sloop() << "</td><td>" << dl->timer.sfull() << "</td>";
            html << "</tr>\n";
        }
        {
            auto dl = &repository.data->serviceNumber;
            html << "<tr><th>serviceNumber</th>";
            html << "<td>" << string_time(dl->time()) << "</td><td>" << dl->size() / 1024 << " Kb</td><td>" <<
            dl->rows() << "</td><td>" << dl->timer.sloop() << "</td><td>" << dl->timer.sfull() << "</td>";
            html << "</tr>\n";
        }
        {
            auto dl = &repository.data->serviceNumberPackage;
            html << "<tr><th>serviceNumberPackage</th>";
            html << "<td>" << string_time(dl->time()) << "</td><td>" << dl->size() / 1024 << " Kb</td><td>" <<
            dl->rows() << "</td><td>" << dl->timer.sloop() << "</td><td>" << dl->timer.sfull() << "</td>";
            html << "</tr>\n";
        }
        {
            auto dl = &repository.data->serviceTrunk;
            html << "<tr><th>serviceTrunk</th>";
            html << "<td>" << string_time(dl->time()) << "</td><td>" << dl->size() / 1024 << " Kb</td><td>" <<
            dl->rows() << "</td><td>" << dl->timer.sloop() << "</td><td>" << dl->timer.sfull() << "</td>";
            html << "</tr>\n";
        }
        {
            auto dl = &repository.data->serviceTrunkPackage;
            html << "<tr><th>serviceTrunkPackage</th>";
            html << "<td>" << string_time(dl->time()) << "</td><td>" << dl->size() / 1024 << " Kb</td><td>" <<
            dl->rows() << "</td><td>" << dl->timer.sloop() << "</td><td>" << dl->timer.sfull() << "</td>";
            html << "</tr>\n";
        }
        {
            auto dl = &repository.data->serviceTrunkSettings;
            html << "<tr><th>serviceTrunkSettings</th>";
            html << "<td>" << string_time(dl->time()) << "</td><td>" << dl->size() / 1024 << " Kb</td><td>" <<
            dl->rows() << "</td><td>" << dl->timer.sloop() << "</td><td>" << dl->timer.sfull() << "</td>";
            html << "</tr>\n";
        }
        {
            auto dl = &repository.data->tariff;
            html << "<tr><th>tariff</th>";
            html << "<td>" << string_time(dl->time()) << "</td><td>" << dl->size() / 1024 << " Kb</td><td>" <<
            dl->rows() << "</td><td>" << dl->timer.sloop() << "</td><td>" << dl->timer.sfull() << "</td>";
            html << "</tr>\n";
        }
        {
            auto dl = &repository.data->tariffPackage;
            html << "<tr><th>tariffPackage</th>";
            html << "<td>" << string_time(dl->time()) << "</td><td>" << dl->size() / 1024 << " Kb</td><td>" <<
            dl->rows() << "</td><td>" << dl->timer.sloop() << "</td><td>" << dl->timer.sfull() << "</td>";
            html << "</tr>\n";
        }
        {
            auto dl = &repository.data->tariffChangeLog;
            html << "<tr><th>tariffChangeLog</th>";
            html << "<td>" << string_time(dl->time()) << "</td><td>" << dl->size() / 1024 << " Kb</td><td>" <<
            dl->rows() << "</td><td>" << dl->timer.sloop() << "</td><td>" << dl->timer.sfull() << "</td>";
            html << "</tr>\n";
        }
        {
            auto dl = &repository.data->statPrefixlist;
            html << "<tr><th>statPrefixlist</th>";
            html << "<td>" << string_time(dl->time()) << "</td><td>" << dl->size() / 1024 << " Kb</td><td>" <<
            dl->rows() << "</td><td>" << dl->timer.sloop() << "</td><td>" << dl->timer.sfull() << "</td>";
            html << "</tr>\n";
        }
        {
            auto dl = &repository.data->statDestinationPrefixlists;
            html << "<tr><th>statDestinationPrefixlists</th>";
            html << "<td>" << string_time(dl->time()) << "</td><td>" << dl->size() / 1024 << " Kb</td><td>" <<
            dl->rows() << "</td><td>" << dl->timer.sloop() << "</td><td>" << dl->timer.sfull() << "</td>";
            html << "</tr>\n";
        }
        {
            auto dl = &repository.data->globalCounters;
            html << "<tr><th>globalCounters</th>";
            html << "<td>" << string_time(dl->time()) << "</td><td>" << dl->size() / 1024 << " Kb</td><td>" <<
            dl->rows() << "</td><td>" << dl->timer.sloop() << "</td><td>" << dl->timer.sfull() << "</td>";
            html << "</tr>\n";
        }
        {
            auto dl = &repository.data->activeCounter;
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
            auto dl = &repository.billingData->clientLock;
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