#include <vector>

#include "../common.h"
#include "Log.h"
#include "RadiusAuthProcessor.h"
#include "BillingCall.h"

struct service_trunk_order {
    bool operator() (const ServiceTrunkOrder & left, const ServiceTrunkOrder & right) {
        return left.price->price < right.price->price;
    }
};

void RadiusAuthProcessor::init() {
    if (!repository.prepare(time(nullptr))) {
        throw Exception("Billing not ready.", "RadiusAuthProcessor::init");
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
            throw Exception("Udp request validation: trunk not found: " + trunkName, "RadiusAuthProcessor::process");
        }



        Cdr cdr;
        cdr.id = 0;
        cdr.connect_time = time(nullptr);
        cdr.session_time = 60;
        strncpy(cdr.src_number, aNumber.c_str(), sizeof(cdr.src_number) - 1);
        strncpy(cdr.dst_number, bNumber.c_str(), sizeof(cdr.dst_number) - 1);
        strncpy(cdr.redirect_number, redirectionNumber.c_str(), sizeof(cdr.redirect_number) - 1);
        strncpy(cdr.src_route, trunkName.c_str(), sizeof(cdr.src_route) - 1);
        strncpy(cdr.dst_route, trunkName.c_str(), sizeof(cdr.dst_route) - 1);
        cdr.src_noa = request.srcNoa;
        cdr.dst_noa = request.dstNoa;
        cdr.call_id = 0;


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
            response.srcNumber = server->calling_station_id_for_line_without_number;
        }

        int outcomeId;


        BillingCall billingCall(&repository);

        Call call = Call(&cdr, CALL_ORIG);
        CallInfo callInfo;
        billingCall.calc(&call, &callInfo, &cdr);
        string billResponse = analyzeCall(call);

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
        response.error += " " + e.getFullMessage();
        e.addTrace("RadiusAuthProcessor ");
        Log::exception(e);
    } catch (std::exception &e) {
        response.error += " " + string(e.what());
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
        throw Exception("Route table #" + lexical_cast<string>(routeTableId) + " not found", "RadiusAuthProcessor::processRouteTable");
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
        throw Exception("Outcome #" + lexical_cast<string>(outcomeId) + " not found", "RadiusAuthProcessor::processOutcome");
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

    throw Exception("Unexpected type of outcome #" + lexical_cast<string>(outcome->id), "RadiusAuthProcessor::processOutcome");
}

void RadiusAuthProcessor::processAutoOutcome(RadiusAuthResponse &response) {

    ServiceTrunk * origServiceTrunk = nullptr;
    Pricelist * origPricelist = nullptr;
    PricelistPrice * origPrice = nullptr;
    getAvailableOrigServiceTrunk(origServiceTrunk, origPricelist, origPrice);


    vector<ServiceTrunkOrder> termServiceTrunks;
    getAvailableTermServiceTrunk(termServiceTrunks);


    processAutoRouteResponse(response, termServiceTrunks);
}

void RadiusAuthProcessor::getAvailableOrigServiceTrunk(ServiceTrunk * origServiceTrunk, Pricelist * origPricelist, PricelistPrice * origPrice) {
    vector<ServiceTrunk *> serviceTrunks;
    repository.getAllServiceTrunk(serviceTrunks, origTrunk->id);

    for (auto serviceTrunk : serviceTrunks) {
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
}

void RadiusAuthProcessor::getAvailableTermServiceTrunk(vector<ServiceTrunkOrder> &termServiceTrunks) {
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

        if (origTrunk->orig_redirect_number && redirectionNumber.size() > 0 && !termTrunk->term_redirect_number) {
            continue;
        }

        filteredTermTrunks.push_back(termTrunk);
    }

    for (auto termTrunk : filteredTermTrunks) {
        vector<ServiceTrunk *> serviceTrunks;
        repository.getAllServiceTrunk(serviceTrunks, termTrunk->id);

        for (auto serviceTrunk : serviceTrunks) {
            Pricelist * pricelist;
            PricelistPrice * price;
            if (checkServiceTrunkAvailability(serviceTrunk, SERVICE_TRUNK_SETTINGS_TERMINATION, pricelist, price)) {
                ServiceTrunkOrder termOrder;
                termOrder.trunk = termTrunk;
                termOrder.serviceTrunk = serviceTrunk;
                termOrder.pricelist = pricelist;
                termOrder.price = price;
                termServiceTrunks.push_back(termOrder);
            }
        }
    }

    sort(termServiceTrunks.begin(), termServiceTrunks.end(), service_trunk_order());
}

void RadiusAuthProcessor::processAutoRouteResponse(RadiusAuthResponse &response, vector<ServiceTrunkOrder> &termOrders) {


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
        throw Exception("Route case #" + lexical_cast<string>(outcome->route_case_id) + " not found", "RadiusAuthProcessor::processRouteCaseOutcome");
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
        throw Exception("Release reason #" + lexical_cast<string>(outcome->release_reason_id) + " not found", "RadiusAuthProcessor::processReleaseReasonOutcome");
    }

    response.setReleaseReason(releaseReason->name);
}

void RadiusAuthProcessor::processAirpOutcome(RadiusAuthResponse &response, Outcome * outcome) {
    auto airp = repository.getAirp(outcome->airp_id);
    if (airp == nullptr) {
        throw Exception("Airp #" + lexical_cast<string>(outcome->airp_id) + " not found", "RadiusAuthProcessor::processAirpOutcome");
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
        throw Exception("Number #" + lexical_cast<string>(numberId) + " not found", "RadiusAuthProcessor::filterByNumber");
    }

    auto prefixlistIds = number->getPrefixlistIds();
    for (auto it = prefixlistIds.begin(); it != prefixlistIds.end(); ++it) {

        auto prefixlist = repository.getPrefixlist(*it);
        if (prefixlist == nullptr) {
            throw Exception("Prefixlist #" + lexical_cast<string>(*it) + " not found", "RadiusAuthProcessor::filterByNumber");
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
        throw Exception("Prefixlist #" + lexical_cast<string>(prefixlistId) + " not found", "RadiusAuthProcessor::matchPrefixlist");
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


string RadiusAuthProcessor::analyzeCall(Call &call) {

    if (call.account_id == 0) {
        return "reject";
    }

    auto client = repository.getAccount(call.account_id);
    if (client == nullptr) {
        return "reject";
    }

    double vat_rate = repository.getVatRate(client);

    double sumBalance = repository.billingData->statsAccountGetSumBalance(call.account_id, vat_rate);
    double sumDay = repository.billingData->statsAccountGetSumDay(call.account_id, vat_rate);

    auto statsAccount2 = repository.currentCalls->getStatsAccount().get();
    double sumBalance2 = statsAccount2->getSumBalance(call.account_id, vat_rate);
    double sumDay2 = statsAccount2->getSumDay(call.account_id, vat_rate);

    double globalBalanceSum, globalDaySum;
    fetchGlobalCounters(call.account_id, globalBalanceSum, globalDaySum, vat_rate);

    double spentBalanceSum, spentDaySum;
    spentBalanceSum = sumBalance + sumBalance2 + globalBalanceSum;
    spentDaySum = sumDay + sumDay2 + globalDaySum;

    if (call.trunk_service_id != 0) {

        if (client->isConsumedCreditLimit(spentBalanceSum)) {
            return "low_balance";
        }

    } else if (call.number_service_id != 0 && call.orig) {

        // Глобальная блокировка
        if (client->is_blocked) {
            return "voip_disabled";
        }

        // Блокировка МГМН
        if (!call.isLocal() && client->disabled) {
            return "voip_disabled";
        }

        // Глобальная блокировка если превышен лимит кредита и не оплачен последний счет
        if (client->isConsumedCreditLimit(spentBalanceSum) && client->last_payed_month < get_tmonth(time(nullptr))) {
            return "low_balance";
        }

        // Блокировка МГМН если превышен лимит кредита
        if (!call.isLocal()  && client->isConsumedCreditLimit(spentBalanceSum)) {
            return "low_balance";
        }

        // Блокировка МГМН если превышен дневной лимит
        if (!call.isLocal() && client->isConsumedDailyLimit(spentDaySum)) {
            return "voip_disabled";
        }

    }

    return "accept";
}


void RadiusAuthProcessor::fetchGlobalCounters(int accountId, double &globalBalanceSum, double &globalDaySum, double vat_rate) {

    GlobalCounters * globalCounter = nullptr;
    if (repository.data->globalCounters.ready()) {
        globalCounter = repository.data->globalCounters.get()->find(accountId);
    }

    if (globalCounter != nullptr) {
        globalBalanceSum = globalCounter->sumBalance(vat_rate);
        globalDaySum = globalCounter->sumDay(vat_rate);
    } else {
        globalBalanceSum = 0.0;
        globalDaySum = 0.0;
    }
}
