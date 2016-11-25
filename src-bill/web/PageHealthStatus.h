#pragma once

#include "BasePage.h"

static const char *statusString[] = {"UNKNOWN", "OK", "WARNING", "ERROR", "CRITICAL"};

class PageHealthStatus : public BasePage {

public:
    bool canHandle(std::string &path) {

        return path == "/health";
    }

    void healthStatusHeader (stringstream &html) {

        html << "<tr>\n";
        html << "<td style='text-align: left' nowrap><b>System</b></td>\n";
        html << "<td style='text-align: left' nowrap><b>Status</b></td>\n";
        html << "<td style='text-align: left' nowrap><b>Error message</b></td>\n";
        html << "</tr>";
    }

    void healthStatusData (stringstream &html, SystemStatus statusData) {

        html << "<tr>\n";
        html << "<td style='text-align: left' nowrap>" << statusData.systemId << "</td>\n";
        html << "<td style='text-align: left' nowrap>" << statusString[statusData.statusId] << "</td>\n";
        html << "<td style='text-align: left' nowrap>" << statusData.statusMessage << "</td>\n";
        html << "</tr>\n";
    }

    void render(std::stringstream &html, map<string, string> &parameters) {

        renderHeader(html);
        std::vector<SystemStatus> systemStatus = app().healthCheckController.getStatus();
        if (systemStatus.empty())
            return;

        html << "<table width=100%>\n";
        healthStatusHeader(html);

        for (auto stat: systemStatus) {

            healthStatusData(html, stat);
        }
        html << "</table>\n";
    }
};