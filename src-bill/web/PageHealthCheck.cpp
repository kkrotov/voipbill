#include "PageHealthCheck.h"

bool PageHealthCheck::canHandle(std::string &path) {

    return path == "/test/healthcheck";
}

void PageHealthCheck::render(std::stringstream &html, map<string, string> &parameters) {

    if (!repository.prepare()) {

        html << "ERROR|BILLING NOT READY";
        return;
    }
    string cmd;
    Json::Value jval;
    if (parameters.find("cmd") != parameters.end()) {

        cmd = parameters["cmd"];
        SystemStatus systemStatus = app().healthCheckController.getStatus(cmd);
        jval["systemId"] = systemStatus.systemId;
        jval["statusId"] = systemStatus.getStatusString();
        jval["statusMessage"] = systemStatus.statusMessage;
        html << jval;
    }
    else {

        std::vector<SystemStatus> systemStatus = app().healthCheckController.getStatus();
        if (systemStatus.empty())
            return;

        for (auto sysstat: systemStatus) {

            jval["systemId"] = sysstat.systemId;
            jval["statusId"] = sysstat.getStatusString();
            jval["statusMessage"] = sysstat.statusMessage;
            html << jval;
        }
    }
}