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

        auto fminCounter = repository.billingData->fminCounter.get();

        if (fminCounter == nullptr) return;

        html << "<table><tr><th>usage_id</th><th>package_id</th><th>month</th><th>seconds</th></tr>";
        for (auto itUsage : fminCounter->counter) {
            int usage_id = itUsage.first;
            map<int, map<time_t, int>> &counterUsage = itUsage.second;

            for (auto itPackage : counterUsage) {
                int package_id = itPackage.first;
                map<time_t, int> &counterMonth = itPackage.second;

                for (auto itMonth : counterMonth) {
                    time_t tMonth = itMonth.first;
                    int seconds = itMonth.second;

                    html
                    << "<tr>"
                    << "<td>" << usage_id << "</td>"
                    << "<td>" << package_id << "</td>"
                    << "<td>" << string_time(tMonth) << "</td>"
                    << "<td>" << seconds << "</td>"
                    << "</tr>";
                }
            }
        }
        html << "</table>";

    }
};