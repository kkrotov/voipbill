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

        auto statsFreemin = &repository.billingData->statsFreemin;
        lock_guard<Spinlock> guard(statsFreemin->lock);

        html << "<table><tr><th>usage_id</th><th>month</th><th>used seconds</th><th>paid seconds</th></tr>";
        for (auto itUsage : statsFreemin->storedFreeminsByServiceId) {
            int usage_id = itUsage.first;
            list<int> &statIds = itUsage.second;

            for (auto statId : statIds) {
                auto itFreemin = statsFreemin->storedStatsFreemin.find(statId);
                if (itFreemin == statsFreemin->storedStatsFreemin.end()) {
                    continue;
                }
                StatsFreemin &stats = itFreemin->second;

                html
                    << "<tr>"
                    << "<td>" << usage_id << "</td>"
                    << "<td>" << string_time(stats.month_dt) << "</td>"
                    << "<td>" << stats.used_seconds << "</td>"
                    << "<td>" << stats.paid_seconds << "</td>"
                    << "</tr>";
            }
        }
        html << "</table>";

    }
};