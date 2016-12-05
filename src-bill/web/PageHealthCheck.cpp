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
    uint16_t instance_id = app().conf.instance_id;
    double run_time = app().getRuntime();

    Json::Value jval;
    if (parameters.find("cmd") != parameters.end()) {

        cmd = parameters["cmd"];
        SystemStatus systemStatus = app().healthCheckController.getStatus(cmd);
        jval["instanceId"] = instance_id;
        jval["itemId"] = systemStatus.itemId;
        jval["itemVal"] = systemStatus.itemValue;
        if (!systemStatus.prevValue.empty())
            jval["itemPrev"] = systemStatus.prevValue;

        if (!systemStatus.nextValue.empty())
            jval["itemNext"] = systemStatus.nextValue;

        jval["statusId"] = systemStatus.getStatusString();
        jval["statusMessage"] = systemStatus.statusMessage;
        jval["runTime"] = run_time;
    }
    else {

        std::vector<SystemStatus> systemStatus = app().healthCheckController.getStatus();
        if (systemStatus.empty())
            return;

        jval["instanceId"] = instance_id;
        jval["runTime"] = run_time;

        int i=0;
        for (auto sysstat: systemStatus) {

            Json::Value jsubval;
            jsubval["itemId"] = sysstat.itemId;
            jsubval["itemVal"] = sysstat.itemValue;
            if (!sysstat.prevValue.empty())
                jsubval["itemPrev"] = sysstat.prevValue;

            if (!sysstat.nextValue.empty())
                jsubval["itemNext"] = sysstat.nextValue;
            
            jsubval["statusId"] = sysstat.getStatusString();
            jsubval["statusMessage"] = sysstat.statusMessage;

            jval["item"+to_string(i++)] = jsubval;
        }
    }
    html << jval;
}