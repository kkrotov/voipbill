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
    uint16_t instance_id = app().conf.instance_id;
    double run_time = app().getRuntime();
    if (parameters.find("cmd") != parameters.end()) {

        cmd = parameters["cmd"];
        SystemStatus systemStatus = app().healthCheckController.getStatus(cmd);
        jval["instanceId"] = instance_id;
        jval["itemId"] = systemStatus.itemId;
        jval["statusId"] = systemStatus.getStatusString();
        jval["statusMessage"] = systemStatus.statusMessage;
        jval["runTime"] = run_time;
        
        html << jval;
    }
    else {

        std::vector<SystemStatus> systemStatus = app().healthCheckController.getStatus();
        if (systemStatus.empty())
            return;

        for (auto sysstat: systemStatus) {

            jval["instanceId"] = instance_id;
            jval["itemId"] = sysstat.itemId;
            jval["statusId"] = sysstat.getStatusString();
            jval["statusMessage"] = sysstat.statusMessage;
            jval["runTime"] = run_time;

            html << jval << std::endl;
        }
    }
}