#pragma once

#include "BasePage.h"
#include "../classes/Repository.h"

class PageFmins : public BasePage {
public:
    bool canHandle(std::string &path) {
        return path == "/fmins";
    }
    void render(std::stringstream &html, map<string, string> &parameters) {
        renderHeader(html);

        Repository repository;

        lock_guard<Spinlock> guard(repository.billingData->lock);
        auto statsFreemin = &repository.billingData->statsFreemin;


        html << "<table><tr><th>usage_id</th><th>month</th><th>used seconds</th></tr>";
        for (auto itUsage : statsFreemin->freeminsByServiceId) {
            int usage_id = itUsage.first;
            list<int> &statIds = itUsage.second;

            for (auto statId : statIds) {
                auto itStats = statsFreemin->statsFreemin.find(statId);
                if (itStats == statsFreemin->statsFreemin.end()) {
                    continue;
                }
                StatsFreemin &stats = itStats->second;

                html << "<tr>";
                html << "<td>" << usage_id << "</td>";
                html << "<td>" << string_time(stats.month_dt) << "</td>";
                html << "<td>" << stats.used_seconds << "</td>";
                html << "</tr>";
            }
        }
        html << "</table>";

    }
};