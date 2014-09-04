#include <vector>

#include "../../src/common.h"
#include "../../src/classes/Log.h"
#include "UdpMessageProcessor.h"
#include "AppAuth.h"
#include "BillClient.h"
#include "ConfigVersionData.h"

UdpMessageProcessor::UdpMessageProcessor(const string &message) {
    this->message = message;

    data = app().getConfigVersionData();
}

void UdpMessageProcessor::parseRequest() {

    vector<string> parameters;
    split(parameters, message, boost::algorithm::is_any_of(";"));

    for (auto it = parameters.begin(); it != parameters.end(); ++it) {
        vector<string> pair;
        split(pair, *it, boost::algorithm::is_any_of(":"));

        if (pair.size() == 2) {
            string name = pair.at(0);
            string value = pair.at(1);

            if (name == "calling") {
                aNumber = value;
            } else if (name == "called") {
                if (value.substr(0, 1) == "0") {
                    prefix = value.substr(0, 3);
                    bNumber = value.substr(3);
                } else {
                    prefix = "";
                    bNumber = value;
                }
            } else if (name == "redirection") {
                redirectionNumber = value;
            } else if (name == "trunk") {
                trunk = atoi(value.c_str());
            }
        }
    }
    
    if (needSwapCallingAndRedirectionNumber() ) {
        string tmp = redirectionNumber;
        redirectionNumber = aNumber;
        aNumber = tmp;
    }
}

bool UdpMessageProcessor::validateRequest() {

    if (data == 0) {
        throw new Exception("ConfigVersionData not ready");
    }

    if (aNumber == "") {
        throw new Exception("Udp request validation: bad calling: " + message, "UdpMessageProcessor::validateRequest");
    }

    if (bNumber == "") {
        throw new Exception("Udp request validation: bad called: " + message, "UdpMessageProcessor::validateRequest");
    }

    if (trunk >= 80) {
        throw new Exception("Udp request validation: bad trunk: " + lexical_cast<string>(trunk), "UdpMessageProcessor::validateRequest");
    }

}

string UdpMessageProcessor::process() {

    try {

        parseRequest();

        validateRequest();
        
        int outcomeId;
        
        string billResponse = BillClient::query(aNumber, bNumber, trunk);
 
        if (billResponse == "voip_disabled") {
            outcomeId = data->version->blocked_outcome_id;           
            if (outcomeId == 0) return "0";
        } else if (billResponse == "low_balance") {
            outcomeId = data->version->low_balance_outcome_id;     
            if (outcomeId == 0) return "0";
        } else if (billResponse == "reject") {
            return "0";
        } else {
            outcomeId = processRouteTable(data->version->route_table_id);
            if (outcomeId == 0) {
                Log::warning("Outcome not found for request: " + message);
                return "1;Cisco-AVPair=Reason=NO_ROUTE_TO_DESTINATION";
            }
        }

        return processOutcome(outcomeId);

    } catch (Exception &e) {
        e.addTrace("UdpMessageProcessor ");
        Log::exception(e);
    } catch (std::exception &e) {
        Log::error("UdpMessageProcessor: " + string(e.what()));
    } catch (...) {
        Log::error("UdpMessageProcessor: ERROR");
    }

    Log::warning("Fallback to default route for request: " + message);
    return string("1");
}

int UdpMessageProcessor::processRouteTable(const int routeTableId) {

    auto routeTable = data->routeTableList->find(routeTableId);
    if (routeTable == 0) {
        throw new Exception("Route table #" + lexical_cast<string>(routeTableId) + " not found", "UdpMessageProcessor::processRouteTable");
    }

    int order = 0;
    while (true) {
        order++;

        auto route = data->routeTableRouteList->find(routeTable->id, order);
        if (route == 0) break;

        if (route->a_number_id && !filterByNumber(route->a_number_id, aNumber)) {
            continue;
        }

        if (route->b_number_id && !filterByNumber(route->b_number_id, bNumber)) {
            continue;
        }

        if (route->outcome_id) {
            return route->outcome_id;
        }

        if (route->outcome_route_table_id) {
            int outcomeId = processRouteTable(route->outcome_route_table_id);
            if (outcomeId != 0) {
                return outcomeId;
            }
        }
    }

    return 0;
}

string UdpMessageProcessor::processOutcome(int outcomeId) {
    auto outcome = data->outcomeList->find(outcomeId);
    if (outcome == 0) {
        throw new Exception("Outcome #" + lexical_cast<string>(outcomeId) + " not found", "UdpMessageProcessor::processOutcome");
    }

    if (outcome->isAuto()) {

        return processAutoOutcome(outcome);

    } else if (outcome->isRouteCase()) {

        return processRouteCaseOutcome(outcome);

    } else if (outcome->isReleaseReason()) {

        return processReleaseReasonOutcome(outcome);

    } else if (outcome->isAirp()) {

        return processAirpOutcome(outcome);

    }

    throw new Exception("Unexpected type of outcome #" + lexical_cast<string>(outcome->id), "UdpMessageProcessor::processOutcome");
}

string UdpMessageProcessor::processAutoOutcome(pOutcome outcome) {
    auto autoRoute = data->routingReportDataList->find(bNumber.c_str());
    if (autoRoute == 0) {
        Log::warning("Auto Route not found for request: " + message);
        return string("1;Cisco-AVPair=Reason=NO_ROUTE_TO_DESTINATION");
    }

    string routeCase = "rc_auto";

    auto operators = autoRoute->getOperators();
    for (auto it = operators.begin(); it != operators.end(); ++it) {
        auto oper = data->operatorList->find(*it);
        if (oper == 0) {
            throw new Exception("Operator #" + lexical_cast<string>(*it) + " not found", "UdpMessageProcessor::processAutoOutcome");
        }

        routeCase = routeCase + "_" + oper->getFormattedCode();
    }

    return string("1;Cisco-AVPair=RTCASE=") + routeCase;
}

string UdpMessageProcessor::processRouteCaseOutcome(pOutcome outcome) {
    auto routeCase = data->routeCaseList->find(outcome->route_case_id);
    if (routeCase == 0) {
        throw new Exception("Route case #" + lexical_cast<string>(outcome->route_case_id) + " not found", "UdpMessageProcessor::processRouteCaseOutcome");
    }

    string response = string("1;Cisco-AVPair=RTCASE=") + string(routeCase->name);
    if (outcome->calling_station_id[0] != 0) {
        response += ";Calling-Station-Id:" + string(outcome->calling_station_id);
    }
    if (outcome->called_station_id[0] != 0) {
        response += ";Called-Station-Id:" + string(outcome->called_station_id);
    }
    return response;
}

string UdpMessageProcessor::processReleaseReasonOutcome(pOutcome outcome) {
    auto releaseReason = data->releaseReasonList->find(outcome->release_reason_id);
    if (releaseReason == 0) {
        throw new Exception("Release reason #" + lexical_cast<string>(outcome->release_reason_id) + " not found", "UdpMessageProcessor::processReleaseReasonOutcome");
    }

    return string("1;Cisco-AVPair=Reason=") + string(releaseReason->name);
}

string UdpMessageProcessor::processAirpOutcome(pOutcome outcome) {
    auto airp = data->airpList->find(outcome->airp_id);
    if (airp == 0) {
        throw new Exception("Airp #" + lexical_cast<string>(outcome->airp_id) + " not found", "UdpMessageProcessor::processAirpOutcome");
    }

    string response = string("1;Cisco-AVPair=AIRP=") + string(airp->name);
    if (outcome->calling_station_id[0] != 0) {
        response += ";Calling-Station-Id:" + string(outcome->calling_station_id);
    }
    if (outcome->called_station_id[0] != 0) {
        response += ";Called-Station-Id:" + string(outcome->called_station_id);
    }
    return response;
}

bool UdpMessageProcessor::filterByNumber(const int numberId, string strNumber) {
    auto number = data->numberList->find(numberId);
    if (number == 0) {
        throw new Exception("Number #" + lexical_cast<string>(numberId) + " not found", "UdpMessageProcessor::filterByNumber");
    }


    auto prefixlistIds = number->getPrefixlistIds();
    for (auto it = prefixlistIds.begin(); it != prefixlistIds.end(); ++it) {

        auto prefixlist = data->prefixlistList->find(*it);
        if (prefixlist == 0) {
            throw new Exception("Prefixlist #" + lexical_cast<string>(*it) + " not found", "UdpMessageProcessor::filterByNumber");
        }

        auto prefix = data->prefixlistPrefixList->find(prefixlist->id, strNumber.c_str());
        if (prefix) {
            return true;
        }
    }

    return false;
}

bool UdpMessageProcessor::needSwapCallingAndRedirectionNumber() {
    if (isConnectedOperator()) {
        return false;
    }
    
    return prefix.substr(2, 1) == "1";
}

bool UdpMessageProcessor::isConnectedOperator() {
    return trunk >= 100;
}
