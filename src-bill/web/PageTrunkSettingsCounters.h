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


        html << "<table><tr><th>id</th><th>trunk_settings_id</th><th>trunk name</th><th>activation<br>expire</th><th>pricelist_id</th><th>minimum_minutes</th><th>minimum_cost</th><th>used mins</th><th>used credit</th></tr>";
        for (auto it : statsTrunkSettings->statsByTrunkSettingsId) {
            int trunkSettingsId = it.first;
            list<int> &statIds = it.second;

            for (auto statId : statIds) {
                auto itStats = statsTrunkSettings->statsTrunkSettings.find(statId);
                if (itStats == statsTrunkSettings->statsTrunkSettings.end()) {
                    continue;
                }
                string trunk_name = "Preparing...";
                int minimum_minutes = 0;
                int minimum_cost = 0;
                int pricelist_id = 0;
                StatsTrunkSettings &stats = itStats->second;
                if (repository.prepare()) {

                    ServiceTrunkSettings trunkSettings;
                    Trunk *trunk = repository.getServiceTrunk(stats.trunk_settings_id, trunkSettings);
                    if (trunk != nullptr) {

                        trunk_name = string(trunk->trunk_name);
                        minimum_minutes = trunkSettings.minimum_minutes;
                        minimum_cost = trunkSettings.minimum_cost;
                        pricelist_id = trunkSettings.pricelist_id;
                    }
                }
                html << "<tr>";
                html << "<td>" << stats.id << "</td>";
                html << "<td>" << stats.trunk_settings_id << "</td>";
                html << "<td>" << trunk_name.c_str() << "</td>";
                html << "<td>" << string_time(stats.activation_dt) << "<br>" << string_time(stats.expire_dt) << "</td>";
                html << "<td>" << pricelist_id << "</td>";
                html << "<td>" << minimum_minutes << "</td>";
                html << "<td>" << minimum_cost << "</td>";
                html << "<td>" << stats.used_seconds/60 << "</td>";
                html << "<td>" << stats.used_credit << "</td>";
                html << "</tr>";
            }
        }
        html << "</table>";

    }
};
