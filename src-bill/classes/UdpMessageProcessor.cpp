#include <vector>

#include "../common.h"
#include "Log.h"
#include "UdpMessageProcessor.h"
#include "BillClient.h"

UdpMessageProcessor::UdpMessageProcessor(const string &message) {
    this->message = message;
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
            } else if (name == "redirnum") {
                redirectionNumber = value;
            } else if (name == "trunk") {
                trunkName = value;
            }
        }
    }

    trunk = repository.getTrunkByName(trunkName.c_str());
    if (trunk == nullptr) {
        throw new Exception("Udp request validation: trunk not found: " + message, "UdpMessageProcessor::parseRequest");
    }

    if (needSwapCallingAndRedirectionNumber()) {
        string tmp = redirectionNumber;
        redirectionNumber = aNumber;
        aNumber = tmp;
    }

    if (trunk->our_trunk
            && server->calling_station_id_for_line_without_number[0] != 0
            && aNumber.substr(0, 11) == string(server->calling_station_id_for_line_without_number)
            && (aNumber.substr(11, 1) == "*" || aNumber.substr(11, 1) == "+")
    ) {
        aNumberForAuth = aNumber.substr(12);
        aNumber = aNumber.substr(0, 11);
    } else {
        aNumberForAuth = aNumber;
    }
}

bool UdpMessageProcessor::validateRequest() {

    if (aNumber == "") {
        throw new Exception("Udp request validation: bad calling: " + message, "UdpMessageProcessor::validateRequest");
    }

    if (bNumber == "") {
        throw new Exception("Udp request validation: bad called: " + message, "UdpMessageProcessor::validateRequest");
    }
}

void UdpMessageProcessor::init() {
    if (!repository.prepare(time(nullptr))) {
        throw new Exception("Billing not ready.", "UdpMessageProcessor::init");
    }

    server = repository.getServer();
}

string UdpMessageProcessor::process() {

    try {
        init();

        parseRequest();

        validateRequest();

        int outcomeId;

        string billResponse = BillClient::query(aNumberForAuth, bNumber, trunk->trunk_name);

        if (billResponse == "voip_disabled") {
            outcomeId = server->blocked_outcome_id;
            if (outcomeId == 0) return "0";
        } else if (billResponse == "low_balance") {
            outcomeId = server->low_balance_outcome_id;
            if (outcomeId == 0) return "0";
        } else if (billResponse == "reject") {
            return "0";
        } else {
            outcomeId = processRouteTable(trunk->route_table_id);
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

    if (trunk == 0) {
        return "1;Cisco-AVPair=Reason=NO_ROUTE_TO_DESTINATION";
    }

    Log::warning("Fallback to default route for request: " + message);
    return string("1");
}

int UdpMessageProcessor::processRouteTable(const int routeTableId) {

    auto routeTable = repository.getRouteTable(routeTableId);
    if (routeTable == 0) {
        throw new Exception("Route table #" + lexical_cast<string>(routeTableId) + " not found", "UdpMessageProcessor::processRouteTable");
    }

    vector<RouteTableRoute *> routes;
    repository.getAllRouteTableRoutes(routes, routeTable->id);

    for (RouteTableRoute * route : routes) {

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
    auto outcome = repository.getOutcome(outcomeId);
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

string UdpMessageProcessor::processAutoOutcome(Outcome * outcome) {

    int operators_count = 0;
    if (operators_count == 0) {
        Log::warning("Auto Route not contains operators for request: " + message);
        return "1;Cisco-AVPair=Reason=NO_ROUTE_TO_DESTINATION";
    }

    string routeCase = "rc_auto";

    return string("1;Cisco-AVPair=RTCASE=") + routeCase;
}

string UdpMessageProcessor::processRouteCaseOutcome(Outcome * outcome) {
    auto routeCase = repository.getRouteCase(outcome->route_case_id);
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

string UdpMessageProcessor::processReleaseReasonOutcome(Outcome * outcome) {
    auto releaseReason = repository.getReleaseReason(outcome->release_reason_id);
    if (releaseReason == 0) {
        throw new Exception("Release reason #" + lexical_cast<string>(outcome->release_reason_id) + " not found", "UdpMessageProcessor::processReleaseReasonOutcome");
    }

    return string("1;Cisco-AVPair=Reason=") + string(releaseReason->name);
}

string UdpMessageProcessor::processAirpOutcome(Outcome * outcome) {
    auto airp = repository.getAirp(outcome->airp_id);
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
    auto number = repository.getNumber(numberId);
    if (number == 0) {
        throw new Exception("Number #" + lexical_cast<string>(numberId) + " not found", "UdpMessageProcessor::filterByNumber");
    }


    auto prefixlistIds = number->getPrefixlistIds();
    for (auto it = prefixlistIds.begin(); it != prefixlistIds.end(); ++it) {

        auto prefixlist = repository.getPrefixlist(*it);
        if (prefixlist == 0) {
            throw new Exception("Prefixlist #" + lexical_cast<string>(*it) + " not found", "UdpMessageProcessor::filterByNumber");
        }

        auto prefix = repository.getPrefixlistPrefix(prefixlist->id, strNumber.c_str());
        if (prefix) {
            return true;
        }
    }

    return false;
}

bool UdpMessageProcessor::needSwapCallingAndRedirectionNumber() {
    return trunk->our_trunk && prefix != "" && prefix.substr(2, 1) == "1";
}

bool UdpMessageProcessor::canRouteForOperator(Trunk * oper) {
    bool matchedForANumber = isOperatorRulesMatched(oper, false, aNumber);
    bool matchedForBNumber = isOperatorRulesMatched(oper, true, bNumber);

    if (oper->source_rule_default_allowed == matchedForANumber) {
        return false;
    }

    if (oper->destination_rule_default_allowed == matchedForBNumber) {
        return false;
    }

    return true;
}


bool UdpMessageProcessor::isOperatorRulesMatched(Trunk * trunk, bool outgoing, string strNumber) {

    vector<TrunkRule *> rules;
    repository.getAllTrunkRules(rules, trunk->id, outgoing);

    for (TrunkRule * rule : rules) {
        auto prefixlist = repository.getPrefixlist(rule->prefixlist_id);
        if (prefixlist == 0) {
            throw new Exception("Prefixlist #" + lexical_cast<string>(rule->prefixlist_id) + " not found", "UdpMessageProcessor::isOperatorRulesMatched");
        }

        auto prefix = repository.getPrefixlistPrefix(prefixlist->id, strNumber.c_str());
        if (prefix) {
            return true;
        }
    }

    return false;
}

