#pragma once

#include "BasePage.h"
#include "../classes/Repository.h"

class PageNNPPackageMinuteCounters : public BasePage {
public:
    bool canHandle(std::string &path) {
        return path == "/nnp-package-minute-counters";
    }

    void render(std::stringstream &html, map<string, string> &parameters) {
        renderHeader(html);

        Repository repository;

        lock_guard<Spinlock> guard(repository.billingData->lock);
        auto statsNNPPackageMinute = &repository.billingData->statsNNPPackageMinute;

        html << "<table><tr>" \
             "<th>id</th>" \
             "<th>np_account_tariff_light_id</th>" \
             "<th>nnp_package_minute_id</th>" \
             "<th>activate_from<br>deactivate_from</th>" \
             "<th>used_seconds</th>" \
             "<th>used_credit</th>" \
             "<th>paid_seconds</th>" \
             "<th>min_call_id</th>" \
             "<th>max_call_id</th></tr>";

        for (auto it : statsNNPPackageMinute->statsByNNPPackageMinuteId) {
            int nnpPackageMinuteId = it.first;
            list<int> &statIds = it.second;

            for (auto statId : statIds) {
                auto itStats = statsNNPPackageMinute->statsNNPPackageMinute.find(statId);
                if (itStats == statsNNPPackageMinute->statsNNPPackageMinute.end()) {
                    continue;
                }
                string trunk_name = "Preparing...";
                int nnp_account_tariff_light_id = 0;
                int nnp_package_minute_id = 0;
                int pricelist_id = 0;
                StatsNNPPackageMinute &stats = itStats->second;

                html << "<tr>";
                html << "<td>" << stats.id << "</td>";
                html << "<td>" << stats.nnp_account_tariff_light_id << "</td>";
                html << "<td>" << stats.nnp_package_minute_id << "</td>";
                html << "<td>" << string_time(stats.activate_from) << "<br>" << string_time(stats.deactivate_from) <<
                "</td>";
                html << "<td>" << stats.used_seconds << "</td>";
                html << "<td>" << stats.used_credit << "</td>";
                html << "<td>" << stats.paid_seconds << "</td>";
                html << "<td>" << stats.min_call_id << "</td>";
                html << "<td>" << stats.max_call_id << "</td>";


                html << "</tr>";
            }
        }
        html << "</table>";

    }
};
