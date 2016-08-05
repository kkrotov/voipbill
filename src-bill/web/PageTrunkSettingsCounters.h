#pragma once

#include "BasePage.h"
#include "../classes/Repository.h"

class PageTrunkSettingsCounters : public BasePage {
public:
    bool canHandle(std::string &path) {
        return path == "/trunk-settings-counters";
    }
    void render(std::stringstream &html, map<string, string> &parameters) {
        renderHeader(html);

        Repository repository;

        lock_guard<Spinlock> guard(repository.billingData->lock);
        auto statsTrunkSettings = &repository.billingData->statsTrunkSettings;


        html << "<table><tr><th>id</th><th>trunk_settings_id</th><th>activation</th><th>expire</th><th>used seconds</th><th>used credit</th></tr>";
        for (auto it : statsTrunkSettings->statsByTrunkSettingsId) {
            int trunkSettingsId = it.first;
            list<int> &statIds = it.second;

            for (auto statId : statIds) {
                auto itStats = statsTrunkSettings->statsTrunkSettings.find(statId);
                if (itStats == statsTrunkSettings->statsTrunkSettings.end()) {
                    continue;
                }
                StatsTrunkSettings &stats = itStats->second;

                html << "<tr>";
                html << "<td>" << stats.id << "</td>";
                html << "<td>" << stats.trunk_settings_id << "</td>";
                html << "<td>" << string_time(stats.activation_dt) << "</td>";
                html << "<td>" << string_time(stats.expire_dt) << "</td>";
                html << "<td>" << stats.used_seconds << "</td>";
                html << "<td>" << stats.used_credit << "</td>";
                html << "</tr>";
            }
        }
        html << "</table>";

    }
};