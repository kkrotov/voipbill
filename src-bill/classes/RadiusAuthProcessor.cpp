#include <vector>

#include "../common.h"
#include "Log.h"
#include "RadiusAuthProcessor.h"
#include "RadiusAuthProcessorBill.h"

struct ServiceTrunkOrder {
    Trunk * trunk;
    ServiceTrunk * serviceTrunk;
    Pricelist * pricelist;
    PricelistPrice * price;
};

void RadiusAuthProcessor::init() {
    if (!repository.prepare(time(nullptr))) {
        throw new Exception("Billing not ready.", "RadiusAuthProcessor::init");
    }

    server = repository.getServer();
}

void RadiusAuthProcessor::process(RadiusAuthRequest &request, RadiusAuthResponse &response) {

    try {
        init();

        aNumber = request.srcNumber;
        if (request.dstNumber.substr(0, 1) == "0") {
            prefix = request.dstNumber.substr(0, 3);
            bNumber = request.dstNumber.substr(3);
        } else {
            prefix = "";
            bNumber = request.dstNumber;
        }

        redirectionNumber = request.redirectNumber;
        trunkName = request.trunkName;

        origTrunk = repository.getTrunkByName(trunkName.c_str());
        if (origTrunk == nullptr) {
            throw new Exception("Udp request validation: trunk not found: " + trunkName, "RadiusAuthProcessor::process");
        }

        if (needSwapCallingAndRedirectionNumber()) {
            string tmp = redirectionNumber;
            redirectionNumber = aNumber;
            aNumber = tmp;
        }

        if (origTrunk->our_trunk
            && server->calling_station_id_for_line_without_number[0] != 0
            && aNumber.substr(0, 11) == string(server->calling_station_id_for_line_without_number)
            && (aNumber.substr(11, 1) == "*" || aNumber.substr(11, 1) == "+")
                ) {
            aNumber = aNumber.substr(0, 11);
        }

        int outcomeId;

        RadiusAuthProcessorBill billProcessor(aNumber, bNumber, trunkName);
        string billResponse = billProcessor.process();

        if (billResponse == "voip_disabled") {
            outcomeId = server->blocked_outcome_id;
            if (outcomeId == 0) {
                response.setReject();
                return;
            }
        } else if (billResponse == "low_balance") {
            outcomeId = server->low_balance_outcome_id;
            if (outcomeId == 0) {
                response.setReject();
                return;
            }
        } else if (billResponse == "reject") {
            response.setReject();
            return;
        } else {
            outcomeId = processRouteTable(origTrunk->route_table_id);
            if (outcomeId == 0) {
                Log::warning("Route table: Outcome not found");
                response.setReleaseReason("NO_ROUTE_TO_DESTINATION");
                return;
            }
        }

        processOutcome(response, outcomeId);
        return;

    } catch (Exception &e) {
        e.addTrace("RadiusAuthProcessor ");
        Log::exception(e);
    } catch (std::exception &e) {
        Log::error("RadiusAuthProcessor: " + string(e.what()));
    } catch (...) {
        Log::error("RadiusAuthProcessor: ERROR");
    }

    if (origTrunk == nullptr) {
        response.setReleaseReason("NO_ROUTE_TO_DESTINATION");
        return;
    }

    Log::warning("Fallback to default route");
    response.setAccept();
}

int RadiusAuthProcessor::processRouteTable(const int routeTableId) {

    auto routeTable = repository.getRouteTable(routeTableId);
    if (routeTable == nullptr) {
        throw new Exception("Route table #" + lexical_cast<string>(routeTableId) + " not found", "RadiusAuthProcessor::processRouteTable");
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

void RadiusAuthProcessor::processOutcome(RadiusAuthResponse &response, int outcomeId) {
    auto outcome = repository.getOutcome(outcomeId);
    if (outcome == nullptr) {
        throw new Exception("Outcome #" + lexical_cast<string>(outcomeId) + " not found", "RadiusAuthProcessor::processOutcome");
    }

    if (outcome->isAuto()) {

        processAutoOutcome(response);
        return;

    } else if (outcome->isRouteCase()) {

        processRouteCaseOutcome(response, outcome);
        return;

    } else if (outcome->isReleaseReason()) {

        processReleaseReasonOutcome(response, outcome);
        return;

    } else if (outcome->isAirp()) {

        processAirpOutcome(response, outcome);
        return;

    }

    throw new Exception("Unexpected type of outcome #" + lexical_cast<string>(outcome->id), "RadiusAuthProcessor::processOutcome");
}

void RadiusAuthProcessor::processAutoOutcome(RadiusAuthResponse &response) {

    vector<ServiceTrunk *> origServiceTrunks;
    repository.getAllServiceTrunk(origServiceTrunks, origTrunk->id);

    ServiceTrunk * origServiceTrunk = nullptr;
    Pricelist * origPricelist = nullptr;
    PricelistPrice * origPrice = nullptr;
    for (auto serviceTrunk : origServiceTrunks) {
        Pricelist * pricelist;
        PricelistPrice * price;
        if (checkServiceTrunkAvailability(serviceTrunk, SERVICE_TRUNK_SETTINGS_ORIGINATION, pricelist, price)) {
            if (origServiceTrunk == nullptr || price->price < origPrice->price) {
                origServiceTrunk = serviceTrunk;
                origPricelist = pricelist;
                origPrice = price;
            }
        }
    }


    vector<Trunk *> termTrunks;
    repository.getAllAutoRoutingTrunks(termTrunks);

    vector<Trunk *> filteredTermTrunks;
    for (auto termTrunk : termTrunks) {
        if (!autoTrunkFilterSrcNumber(termTrunk)) {
            continue;
        }
        if (!autoTrunkFilterDstNumber(termTrunk)) {
            continue;
        }
        filteredTermTrunks.push_back(termTrunk);
    }

    vector<ServiceTrunkOrder> termOrders;

    for (auto termTrunk : filteredTermTrunks) {
        vector<ServiceTrunk *> termServiceTrunks;
        repository.getAllServiceTrunk(termServiceTrunks, termTrunk->id);

        for (auto serviceTrunk : termServiceTrunks) {
            Pricelist * pricelist;
            PricelistPrice * price;
            if (checkServiceTrunkAvailability(serviceTrunk, SERVICE_TRUNK_SETTINGS_TERMINATION, pricelist, price)) {
                ServiceTrunkOrder termOrder;
                termOrder.trunk = termTrunk;
                termOrder.serviceTrunk = serviceTrunk;
                termOrder.pricelist = pricelist;
                termOrder.price = price;
                termOrders.push_back(termOrder);
            }
        }
    }

    struct service_trunk_order {
        bool operator() (const ServiceTrunkOrder & left, const ServiceTrunkOrder & right) {
            return left.price->price < right.price->price;
        }
    };

    sort(termOrders.begin(), termOrders.end(), service_trunk_order());

    if (termOrders.size() == 0) {
        Log::warning("Auto Route not contains operators");
        response.setReleaseReason("NO_ROUTE_TO_DESTINATION");
        return;
    }

    string routeCase;

    int n = 0;
    for (ServiceTrunkOrder &trunkOrder : termOrders) {
        if (++n > 3) break;

        routeCase += "_" + lexical_cast<string>(trunkOrder.trunk->id);
    }

    response.setRouteCase("rc_auto" + routeCase);
}

void RadiusAuthProcessor::processRouteCaseOutcome(RadiusAuthResponse &response, Outcome * outcome) {
    auto routeCase = repository.getRouteCase(outcome->route_case_id);
    if (routeCase == nullptr) {
        throw new Exception("Route case #" + lexical_cast<string>(outcome->route_case_id) + " not found", "RadiusAuthProcessor::processRouteCaseOutcome");
    }

    response.setRouteCase(routeCase->name);

    if (outcome->calling_station_id[0] != 0) {
        response.srcNumber = outcome->calling_station_id;
    }
    if (outcome->called_station_id[0] != 0) {
        response.dstNumber = outcome->called_station_id;
    }
}

void RadiusAuthProcessor::processReleaseReasonOutcome(RadiusAuthResponse &response, Outcome * outcome) {
    auto releaseReason = repository.getReleaseReason(outcome->release_reason_id);
    if (releaseReason == nullptr) {
        throw new Exception("Release reason #" + lexical_cast<string>(outcome->release_reason_id) + " not found", "RadiusAuthProcessor::processReleaseReasonOutcome");
    }

    response.setReleaseReason(releaseReason->name);
}

void RadiusAuthProcessor::processAirpOutcome(RadiusAuthResponse &response, Outcome * outcome) {
    auto airp = repository.getAirp(outcome->airp_id);
    if (airp == nullptr) {
        throw new Exception("Airp #" + lexical_cast<string>(outcome->airp_id) + " not found", "RadiusAuthProcessor::processAirpOutcome");
    }

    response.setAirp(airp->name);

    if (outcome->calling_station_id[0] != 0) {
        response.srcNumber = outcome->calling_station_id;
    }
    if (outcome->called_station_id[0] != 0) {
        response.dstNumber = outcome->called_station_id;
    }
}

bool RadiusAuthProcessor::filterByNumber(const int numberId, string strNumber) {
    auto number = repository.getNumber(numberId);
    if (number == nullptr) {
        throw new Exception("Number #" + lexical_cast<string>(numberId) + " not found", "RadiusAuthProcessor::filterByNumber");
    }

    auto prefixlistIds = number->getPrefixlistIds();
    for (auto it = prefixlistIds.begin(); it != prefixlistIds.end(); ++it) {

        auto prefixlist = repository.getPrefixlist(*it);
        if (prefixlist == nullptr) {
            throw new Exception("Prefixlist #" + lexical_cast<string>(*it) + " not found", "RadiusAuthProcessor::filterByNumber");
        }

        auto prefix = repository.getPrefixlistPrefix(prefixlist->id, strNumber.c_str());
        if (prefix) {
            return true;
        }
    }

    return false;
}

bool RadiusAuthProcessor::needSwapCallingAndRedirectionNumber() {
    return origTrunk->our_trunk && prefix != "" && prefix.substr(2, 1) == "1";
}

bool RadiusAuthProcessor::autoTrunkFilterSrcNumber(Trunk * termTrunk) {
    vector<TrunkRule *> rules;
    repository.getAllTrunkRules(rules, termTrunk->id, false);
    if (termTrunk->source_rule_default_allowed) {
        for (TrunkRule * rule : rules) {
            if (matchPrefixlist(rule->prefixlist_id, aNumber)) {
                return false;
            }
        }
        return true;
    } else {
        for (TrunkRule * rule : rules) {
            if (matchPrefixlist(rule->prefixlist_id, aNumber)) {
                return true;
            }
        }
        return false;
    }
}

bool RadiusAuthProcessor::autoTrunkFilterDstNumber(Trunk * termTrunk) {
    vector<TrunkRule *> rules;
    repository.getAllTrunkRules(rules, termTrunk->id, true);
    if (termTrunk->destination_rule_default_allowed) {
        for (TrunkRule * rule : rules) {
            if (matchPrefixlist(rule->prefixlist_id, bNumber)) {
                return false;
            }
        }
        return true;
    } else {
        for (TrunkRule * rule : rules) {
            if (matchPrefixlist(rule->prefixlist_id, bNumber)) {
                return true;
            }
        }
        return false;
    }
}

bool RadiusAuthProcessor::matchPrefixlist(const int prefixlistId, string strNumber) {
    auto prefixlist = repository.getPrefixlist(prefixlistId);
    if (prefixlist == nullptr) {
        throw new Exception("Prefixlist #" + lexical_cast<string>(prefixlistId) + " not found", "RadiusAuthProcessor::matchPrefixlist");
    }

    auto prefix = repository.getPrefixlistPrefix(prefixlist->id, strNumber.c_str());
    return prefix != nullptr;
}

bool RadiusAuthProcessor::checkServiceTrunkAvailability(ServiceTrunk *serviceTrunk, int type, Pricelist * &pricelist, PricelistPrice * &price) {

    vector<ServiceTrunkSettings *> serviceTrunkSettings;
    repository.getAllServiceTrunkSettings(serviceTrunkSettings, serviceTrunk->id, type);
    for (auto trunkSettings : serviceTrunkSettings) {

        if (trunkSettings->src_number_id > 0 && !repository.matchNumber(trunkSettings->src_number_id, atoll(aNumber.c_str()))) {
            continue;
        }

        if (trunkSettings->dst_number_id > 0 && !repository.matchNumber(trunkSettings->dst_number_id, atoll(bNumber.c_str()))) {
            continue;
        }

        pricelist = repository.getPricelist(trunkSettings->pricelist_id);
        if (pricelist == nullptr) {
            continue;
        }

        if (pricelist->local) {

            auto networkPrefix = repository.getNetworkPrefix(pricelist->local_network_config_id, atoll(bNumber.c_str()));
            if (networkPrefix == nullptr) {
                continue;
            }

            price = repository.getPrice(trunkSettings->pricelist_id, networkPrefix->network_type_id);
            if (price == nullptr) {
                continue;
            }


        } else {

            price = repository.getPrice(trunkSettings->pricelist_id, atoll(bNumber.c_str()));
            if (price == nullptr) {
                continue;
            }

        }

        return true;
    }

    return false;
}
