#pragma once

#include "BasePage.h"
#include "../classes/Repository.h"

class PagePackage : public BasePage {
public:
    bool canHandle(std::string &path) {
        return path == "/packages";
    }
    void render(std::stringstream &html, map<string, string> &parameters) {
        renderHeader(html);

        Repository repository;

        lock_guard<Spinlock> guard(repository.billingData->lock);
        auto statsPackage = &repository.billingData->statsPackage;


        html << "<table><tr><th>id</th><th>package</th><th>activation</th><th>expire</th><th>paid seconds</th><th>used seconds</th><th>used credit</th></tr>";
        for (auto it : statsPackage->statsByPackageId) {
            int package_id = it.first;
            list<int> &statIds = it.second;

            for (auto statId : statIds) {
                auto itStats = statsPackage->statsPackage.find(statId);
                if (itStats == statsPackage->statsPackage.end()) {
                    continue;
                }
                StatsPackage &stats = itStats->second;

                html << "<tr>";
                html << "<td>" << stats.id << "</td>";
                html << "<td>" << stats.package_id << "</td>";
                html << "<td>" << string_time(stats.activation_dt) << "</td>";
                html << "<td>" << string_time(stats.expire_dt) << "</td>";
                html << "<td>" << stats.paid_seconds << "</td>";
                html << "<td>" << stats.used_seconds << "</td>";
                html << "<td>" << stats.used_credit << "</td>";
                html << "</tr>";
            }
        }
        html << "</table>";

    }
};