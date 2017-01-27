#include "PageHealthCheck.h"

bool PageHealthCheck::canHandle(std::string &path) {

    return path == "/test/healthcheck";
}

vector<pair<int,string>> PageHealthCheck::getRegionList() {

    vector<pair<int,string>> regionList;
    if (repository.prepare()) {

        int server_id = app().conf.instance_id;
        int hub_id = app().conf.hub_id;

        if (hub_id > 0) {

            vector<Server> servers;
            repository.getServersByHubId(servers, hub_id);
            for (auto server: servers) {

                regionList.push_back(pair<int,string>(server.id,server.name));
            }
        }
        if (regionList.size()==0) {

            Server *server = repository.getServer(server_id);
            if(server!= nullptr)
                regionList.push_back(pair<int,string>(server->id,server->name));
        }
    }
    return regionList;
}

Json::Value PageHealthCheck::getJsonRegionList() {

    Json::Value jval;
    vector<pair<int,string>> regionList = getRegionList();
    for (auto reg: regionList) {

        Json::Value jsubval;
        jsubval["regionId"] = reg.first;
        jsubval["regionName"] = reg.second;

        jval.append(jsubval);
    }
    return jval;
}

void PageHealthCheck::render(std::stringstream &html, map<string, string> &parameters) {

    string cmd;
    uint16_t instance_id = app().conf.instance_id;
    double run_time = app().getRuntime();
    int currentCalls = 0;
    if (repository.prepare()) {

        shared_ptr<CurrentCdrList> cdrList = repository.currentCalls->currentCdr.get();
        currentCalls = (cdrList== nullptr)? 0:cdrList->size();
    }
    Json::Value jval;
    jval["instanceId"] = instance_id;
    jval["regionList"] = getJsonRegionList();
    jval["currentCalls"] = currentCalls;
    jval["runTime"] = run_time;
    if (parameters.find("cmd") != parameters.end()) {

        cmd = parameters["cmd"];
        SystemStatus systemStatus = app().healthCheckController.getStatus(cmd);
        jval["itemId"] = systemStatus.itemId;
        jval["itemVal"] = systemStatus.itemValue;
        if (!systemStatus.prevValue.empty())
            jval["itemPrev"] = systemStatus.prevValue;

        if (!systemStatus.nextValue.empty())
            jval["itemNext"] = systemStatus.nextValue;

        jval["statusId"] = systemStatus.getStatusString();
        jval["statusMessage"] = systemStatus.statusMessage;
    }
    else {

        std::vector<SystemStatus> systemStatus = app().healthCheckController.getStatus();
        if (systemStatus.empty())
            return;

        int i=0;
        for (auto sysstat: systemStatus) {

            if (sysstat.statusId==HealthStatus::STATUS_UNKNOWN)
                continue;

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