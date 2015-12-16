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
    if (!repository.prepare() || !repository.billingData->ready()) {
        throw Exception("Billing not ready.", "RadiusAuthProcessor::init");
    }

    server = repository.getServer();

    origTrunk = repository.getTrunkByName(request->trunkName.c_str());
    if (origTrunk == nullptr) {
        throw Exception("Udp request validation: trunk not found: " + request->trunkName, "RadiusAuthProcessor::process");
    }

    aNumber = request->srcNumber;
    bNumber = request->dstNumber;

}

RadiusAuthProcessor::RadiusAuthProcessor(RadiusAuthRequest * request, RadiusAuthResponse *response, pLogMessage &logRequest) {
    this->request = request;
    this->response = response;
    this->logRequest = logRequest;
    this->trace = nullptr;
    this->account = nullptr;
}

void RadiusAuthProcessor::setTrace(stringstream *trace) {
    this->trace = trace;
    repository.trace = trace;
}

void RadiusAuthProcessor::process() {

    try {
        init();

        processRedirectNumber();
        processLineWithoutNumber();

        Cdr cdr;
        cdr.connect_time = time(nullptr);
        cdr.session_time = 60;
        strncpy(cdr.src_number, request->srcNumber.c_str(), sizeof(cdr.src_number) - 1);
        strncpy(cdr.dst_number, request->dstNumber.c_str(), sizeof(cdr.dst_number) - 1);
        strncpy(cdr.redirect_number, request->redirectNumber.c_str(), sizeof(cdr.redirect_number) - 1);
        strncpy(cdr.src_route, request->trunkName.c_str(), sizeof(cdr.src_route) - 1);
        strncpy(cdr.dst_route, request->trunkName.c_str(), sizeof(cdr.dst_route) - 1);
        cdr.src_noa = request->srcNoa;
        cdr.dst_noa = request->dstNoa;


        int outcomeId;

        BillingCall billingCall(&repository);

        Call call = Call(&cdr, CALL_ORIG);
        CallInfo callInfo;
        billingCall.calc(&call, &callInfo, &cdr);

        account = callInfo.account;

        if (trace != nullptr) {
            *trace << "INFO|CALL|";
            call.dump(*trace);
            *trace << "\n";
        }

        string billResponse = analyzeCall(call);
        if (trace != nullptr) {
            *trace << "INFO|BILL RESPONSE|" << billResponse << "\n";
        }

        logRequest->params["orig"] = call.orig ? "true" : "false";
        logRequest->params["src"] = call.src_number;
        logRequest->params["dst"] = call.dst_number;
        logRequest->params["trunk_id"] = call.trunk_id;
        logRequest->params["account_id"] = call.account_id;
        logRequest->params["trunk_service_id"] = call.trunk_service_id;
        logRequest->params["number_service_id"] = call.number_service_id;
        logRequest->params["service_package_id"] = call.service_package_id;
        logRequest->params["pricelist_id"] = call.pricelist_id;
        logRequest->params["pricelist_prefix"] = lexical_cast<string>(call.prefix);
        logRequest->params["geo_id"] = call.geo_id;
        logRequest->params["rate"] = call.rate;
        logRequest->params["cost"] = call.cost;

        if (callInfo.account != nullptr) {
            double vat_rate = repository.getVatRate(callInfo.account);

            double sumBalance = repository.billingData->statsAccountGetSumBalance(callInfo.account->id, vat_rate);
            double sumDay = repository.billingData->statsAccountGetSumDay(callInfo.account->id, vat_rate);

            auto statsAccount2 = repository.currentCalls->getStatsAccount().get();
            double sumBalance2 = statsAccount2->getSumBalance(callInfo.account->id, vat_rate) + call.cost;
            double sumDay2 = statsAccount2->getSumDay(callInfo.account->id, vat_rate) + call.cost;

            double globalBalanceSum, globalDaySum;
            fetchGlobalCounters(callInfo.account->id, globalBalanceSum, globalDaySum, vat_rate);

            double spentBalanceSum, spentDaySum;
            spentBalanceSum = sumBalance + sumBalance2 + globalBalanceSum;
            spentDaySum = sumDay + sumDay2 + globalDaySum;

            logRequest->params["balance_stat"] = callInfo.account->balance;
            logRequest->params["balance_local"] = sumBalance;
            logRequest->params["balance_current"] = sumBalance2;
            logRequest->params["balance_global"] = globalBalanceSum;
            logRequest->params["balance_realtime"] = callInfo.account->balance + spentBalanceSum;
            if (callInfo.account->hasCreditLimit()) {
                logRequest->params["credit_limit"] = callInfo.account->credit;
                logRequest->params["credit_available"] = callInfo.account->balance + callInfo.account->credit + spentBalanceSum;
            }

            logRequest->params["daily_local"] = sumDay;
            logRequest->params["daily_current"] = sumDay2;
            logRequest->params["daily_global"] = globalDaySum;
            logRequest->params["daily_total"] = spentDaySum;
            if (callInfo.account->hasDailyLimit()) {
                logRequest->params["daily_limit"] = callInfo.account->limit_d;
                logRequest->params["daily_available"] = callInfo.account->limit_d + spentDaySum;
            }

            if (callInfo.account->is_blocked) {
                logRequest->params["block_full_flag"] = "true";
            }

            if (callInfo.account->disabled) {
                logRequest->params["block_mgmn_flag"] = "true";
            }

            if (callInfo.account->isConsumedCreditLimit(spentBalanceSum)) {
                logRequest->params["block_credit_flag"] = "true";
            }

            if (callInfo.account->isConsumedDailyLimit(spentDaySum)) {
                logRequest->params["block_daily_flag"] = "true";
            }
        }

        logRequest->params["resp_bill"] = billResponse;

        if (billResponse == "voip_disabled") {
            if (trace != nullptr) {
                *trace << "INFO|SPECIAL OUTCOME|VOIP DISABLED" << "\n";
            }
            outcomeId = server->blocked_outcome_id;
            if (outcomeId == 0) {
                response->setReject();
                return;
            }
        } else if (billResponse == "low_balance") {
            if (trace != nullptr) {
                *trace << "INFO|SPECIAL OUTCOME|LOW BALANCE" << "\n";
            }
            outcomeId = server->low_balance_outcome_id;
            if (outcomeId == 0) {
                response->setReject();
                return;
            }
        } else if (billResponse == "reject") {
            if (trace != nullptr) {
                *trace << "INFO|REJECT" << "\n";
            }
            response->setReject();
            return;
        } else {
            outcomeId = processRouteTable(origTrunk->route_table_id);
            if (outcomeId == 0) {
                Log::warning("Route table: Outcome not found");
                response->setReleaseReason("NO_ROUTE_TO_DESTINATION");
                return;
            }
        }

        processOutcome(outcomeId);
        return;

    } catch (Exception &e) {
        response->error += " " + e.getFullMessage();
        e.addTrace("RadiusAuthProcessor ");
        Log::exception(e);
    } catch (std::exception &e) {
        response->error += " " + string(e.what());
        Log::error("RadiusAuthProcessor: " + string(e.what()));
    } catch (...) {
        Log::error("RadiusAuthProcessor: ERROR");
    }

    if (origTrunk == nullptr) {
        response->setReleaseReason("NO_ROUTE_TO_DESTINATION");
        return;
    }

    Log::warning("Fallback to default route");
    response->setAccept();
}

int RadiusAuthProcessor::processRouteTable(const int routeTableId) {

    auto routeTable = repository.getRouteTable(routeTableId);
    if (routeTable == nullptr) {
        throw Exception("Route table #" + lexical_cast<string>(routeTableId) + " not found", "RadiusAuthProcessor::processRouteTable");
    }


    if (trace != nullptr) {
        *trace << "INFO|ROUTE TABLE|" << routeTable->name << " (" << routeTableId << ")" << "\n";
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

void RadiusAuthProcessor::processOutcome(int outcomeId) {
    auto outcome = repository.getOutcome(outcomeId);
    if (outcome == nullptr) {
        throw Exception("Outcome #" + lexical_cast<string>(outcomeId) + " not found", "RadiusAuthProcessor::processOutcome");
    }

    if (trace != nullptr) {
        *trace << "INFO|OUTCOME|" << outcome->name << " (" << outcomeId << ")" << "\n";
    }


    if (outcome->isAuto()) {

        processAutoOutcome();
        return;

    } else if (outcome->isRouteCase()) {

        processRouteCaseOutcome(outcome);
        return;

    } else if (outcome->isReleaseReason()) {

        processReleaseReasonOutcome(outcome);
        return;

    } else if (outcome->isAirp()) {

        processAirpOutcome(outcome);
        return;

    }

    throw Exception("Unexpected type of outcome #" + lexical_cast<string>(outcome->id), "RadiusAuthProcessor::processOutcome");
}

void RadiusAuthProcessor::processAutoOutcome() {

    ServiceTrunk * origServiceTrunk = nullptr;
    Pricelist * origPricelist = nullptr;
    PricelistPrice * origPrice = nullptr;
    getAvailableOrigServiceTrunk(origServiceTrunk, origPricelist, origPrice);


    vector<ServiceTrunkOrder> termServiceTrunks;
    getAvailableTermServiceTrunk(termServiceTrunks);


    processAutoRouteResponse(termServiceTrunks);
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

    for (auto termTrunk : termTrunks) {
        if (!autoTrunkFilterSrcTrunk(termTrunk)) {
            if (trace != nullptr) {
                *trace << "INFO|TERM SERVICE TRUNK DECLINE|BY TRUNK FILTER, " << termTrunk->name << " (" << termTrunk->id << ")" << "\n";
            }
            continue;
        }
        if (!autoTrunkFilterSrcNumber(termTrunk)) {
            if (trace != nullptr) {
                *trace << "INFO|TERM SERVICE TRUNK DECLINE|BY SRC NUMBER FILTER, " << termTrunk->name << " (" << termTrunk->id << ")" << "\n";
            }
            continue;
        }
        if (!autoTrunkFilterDstNumber(termTrunk)) {
            if (trace != nullptr) {
                *trace << "INFO|TERM SERVICE TRUNK DECLINE|BY DST NUMBER FILTER, " << termTrunk->name << " (" << termTrunk->id << ")" << "\n";
            }
            continue;
        }

        if (origTrunk->orig_redirect_number && request->redirectNumber.size() > 0 && !termTrunk->term_redirect_number) {
            if (trace != nullptr) {
                *trace << "INFO|TERM SERVICE TRUNK DECLINE|CAUSE NOT SUPPORT REDIRECTING NUMBER, " << termTrunk->name << " (" << termTrunk->id << ")" << "\n";
            }
            continue;
        }

        vector<ServiceTrunk *> serviceTrunks;
        repository.getAllServiceTrunk(serviceTrunks, termTrunk->id);

        if (serviceTrunks.size() == 0) {
            if (trace != nullptr) {
                *trace << "INFO|TERM SERVICE TRUNK DECLINE|CAUSE SERVICE TRUNK NOT FOUND, " << termTrunk->name << " (" << termTrunk->id << ")" << "\n";
            }
            continue;
        }


        for (auto serviceTrunk : serviceTrunks) {
            Pricelist * pricelist;
            PricelistPrice * price;
            if (checkServiceTrunkAvailability(serviceTrunk, SERVICE_TRUNK_SETTINGS_TERMINATION, pricelist, price)) {
                if (
                    server->min_price_for_autorouting > 0
                    && price->price > server->min_price_for_autorouting
                    && account != nullptr
                    && !account->anti_fraud_disabled)
                {
                    if (trace != nullptr) {
                        *trace << "INFO|TERM SERVICE TRUNK DECLINE|CAUSE ANTI FRAUD: " << termTrunk->name << " (" << termTrunk->id << ")" << ", SERVICE TRUNK ID: "  << serviceTrunk->id << "\n";
                    }
                } else {
                    ServiceTrunkOrder termOrder;
                    termOrder.trunk = termTrunk;
                    termOrder.serviceTrunk = serviceTrunk;
                    termOrder.pricelist = pricelist;
                    termOrder.price = price;
                    termServiceTrunks.push_back(termOrder);
                    if (trace != nullptr) {
                        *trace << "INFO|TERM SERVICE TRUNK ACCEPT|" << termTrunk->name << " (" << termTrunk->id << ")" << ", SERVICE TRUNK ID: "  << serviceTrunk->id << "\n";
                    }
                }
            } else {
                if (trace != nullptr) {
                    *trace << "INFO|TERM SERVICE TRUNK DECLINE|" << termTrunk->name << " (" << termTrunk->id << ")" << ", SERVICE TRUNK ID: "  << serviceTrunk->id << "\n";
                }
            }
        }
    }

    sort(termServiceTrunks.begin(), termServiceTrunks.end(), service_trunk_order());
}

void RadiusAuthProcessor::processAutoRouteResponse(vector<ServiceTrunkOrder> &termOrders) {

    if (termOrders.size() == 0) {
        Log::warning("Auto Route not contains operators");
        response->setReleaseReason("NO_ROUTE_TO_DESTINATION");
        return;
    }

    if (trace != nullptr) {
        *trace << "INFO|TERM SERVICE TRUNKS|" << "\n";
    }

    for (ServiceTrunkOrder &trunkOrder : termOrders) {
        if (trace != nullptr) {
            *trace << "INFO||PRICE: " << trunkOrder.price->price;
            *trace << ", TRUNK: " << trunkOrder.trunk->name << " (" << trunkOrder.trunk->id << ")";
            *trace << ", SERVICE TRUNK " << trunkOrder.serviceTrunk->id;
            *trace << ", PRICELIST: " << trunkOrder.pricelist->id;
            *trace << ", PREFIX: " << trunkOrder.price->prefix;
            *trace << "\n";
        }
    }

    string routeCase;
    set<int> trunkIds;

    for (ServiceTrunkOrder &trunkOrder : termOrders) {
        if (trunkIds.find(trunkOrder.trunk->id) != trunkIds.end()) {
            continue;
        }
        trunkIds.insert(trunkOrder.trunk->id);

        routeCase += "_" + lexical_cast<string>(trunkOrder.trunk->id);

        if (trunkIds.size() == 3) {
            break;
        }
    }

    response->setRouteCase("rc_auto" + routeCase);
}

void RadiusAuthProcessor::processRouteCaseOutcome(Outcome * outcome) {
    auto routeCase = repository.getRouteCase(outcome->route_case_id);
    if (routeCase == nullptr) {
        throw Exception("Route case #" + lexical_cast<string>(outcome->route_case_id) + " not found", "RadiusAuthProcessor::processRouteCaseOutcome");
    }

    response->setRouteCase(routeCase->name);

    if (outcome->calling_station_id[0] != 0) {
        response->srcNumber = outcome->calling_station_id;
    }
    if (outcome->called_station_id[0] != 0) {
        response->dstNumber = outcome->called_station_id;
    }
}

void RadiusAuthProcessor::processReleaseReasonOutcome(Outcome * outcome) {
    auto releaseReason = repository.getReleaseReason(outcome->release_reason_id);
    if (releaseReason == nullptr) {
        throw Exception("Release reason #" + lexical_cast<string>(outcome->release_reason_id) + " not found", "RadiusAuthProcessor::processReleaseReasonOutcome");
    }

    response->setReleaseReason(releaseReason->name);
}

void RadiusAuthProcessor::processAirpOutcome(Outcome * outcome) {
    auto airp = repository.getAirp(outcome->airp_id);
    if (airp == nullptr) {
        throw Exception("Airp #" + lexical_cast<string>(outcome->airp_id) + " not found", "RadiusAuthProcessor::processAirpOutcome");
    }

    response->setAirp(airp->name);

    if (outcome->calling_station_id[0] != 0) {
        response->srcNumber = outcome->calling_station_id;
    }
    if (outcome->called_station_id[0] != 0) {
        response->dstNumber = outcome->called_station_id;
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
            if (trace != nullptr) {
                *trace << "DEBUG|PREFIXLIST MATCHED|" << strNumber  << " in "<< prefixlist->name << " (" << prefixlist->id << ")" << "\n";
            }
            if (trace != nullptr) {
                *trace << "INFO|NUMBER MATCHED|" << strNumber  << " in "<< number->name << " (" << numberId << ")" << "\n";
            }
            return true;
        }

        if (trace != nullptr) {
            *trace << "DEBUG|PREFIXLIST NOT MATCHED|" << strNumber  << " not in "<< prefixlist->name << " (" << prefixlist->id << ")" << "\n";
        }
    }

    if (trace != nullptr) {
        *trace << "INFO|NUMBER NOT MATCHED|" << strNumber  << " not in "<< number->name << " (" << numberId << ")" << "\n";
    }

    return false;
}

void RadiusAuthProcessor::processRedirectNumber() {
    if (origTrunk->orig_redirect_number && request->redirectNumber.size() > 0) {
        aNumber = request->redirectNumber;
        if (trace != nullptr) {
            *trace << "INFO|REDIRECT NUMBER|SET REQUEST.CALLING = " << aNumber << "\n";
        }
    }
}

void RadiusAuthProcessor::processLineWithoutNumber() {
    if (origTrunk->our_trunk
        && origTrunk->auth_by_number
        && server->calling_station_id_for_line_without_number[0] != 0
        && (aNumber.substr(11, 1) == "*" || aNumber.substr(11, 1) == "+")
    ) {
        response->srcNumber = server->calling_station_id_for_line_without_number;
        if (trace != nullptr) {
            *trace << "INFO|LINE WITH NUMBER|SET RESPONSE.CALLING = " << response->srcNumber << "\n";
        }
    }
}

bool RadiusAuthProcessor::autoTrunkFilterSrcTrunk(Trunk * termTrunk) {
    vector<int> resultTrunkGroupIds;
    repository.getAllTrunkTrunkRulesTrunkGroupIds(resultTrunkGroupIds, termTrunk->id);

    if (termTrunk->source_trunk_rule_default_allowed) {
        for (int trunkGroupId : resultTrunkGroupIds) {
            if (matchTrunkGroup(trunkGroupId, origTrunk->id)) {
                return false;
            }
        }
        return true;
    } else {
        for (int trunkGroupId : resultTrunkGroupIds) {
            if (matchTrunkGroup(trunkGroupId, origTrunk->id)) {
                return true;
            }
        }
        return false;
    }
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

bool RadiusAuthProcessor::matchTrunkGroup(const int trunkGroupId, const int matchTrunkId) {

    auto trunkGroup = repository.getTrunkGroup(trunkGroupId);
    if (trunkGroup == nullptr) {
        throw Exception("TrunkGroup #" + lexical_cast<string>(trunkGroupId) + " not found", "RadiusAuthProcessor::matchTrunkGroup");
    }

    vector<int> trunkIds;
    repository.getAllTrunkGroupTrunkIds(trunkIds, trunkGroupId);
    for (int trunkId : trunkIds) {
        if (trunkId == matchTrunkId) {
            if (trace != nullptr) {
                *trace << "DEBUG|TRUNK GROUP MATCHED|" << trunkGroup->name << "(" << trunkGroup->id << "): " << matchTrunkId << " in ";
                for (int tmpId : trunkIds) *trace << tmpId << ",";
                *trace << "\n";
            }
            return true;
        }
    }

    if (trace != nullptr) {
        *trace << "DEBUG|TRUNK GROUP NOT MATCHED|" << trunkGroup->name << "(" << trunkGroup->id << "): " << matchTrunkId << " not in ";
        for (int tmpId : trunkIds) *trace << tmpId << ",";
        *trace << "\n";
    }
    return false;
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

    if (serviceTrunkSettings.size() == 0) {
        if (trace != nullptr) {
            *trace << "DEBUG|SERVICE TRUNK DECLINE|TRUNK SETTINGS NOT FOUND, SEVICE_TRUNK_ID: " << serviceTrunk->id << "\n";
        }
        return false;
    }

    for (auto trunkSettings : serviceTrunkSettings) {

        if (trunkSettings->src_number_id > 0 && !repository.matchNumber(trunkSettings->src_number_id, atoll(aNumber.c_str()))) {
            if (trace != nullptr) {
                *trace << "DEBUG|SERVICE TRUNK DECLINE|BY SRC NUMBER MATCHING, SEVICE_TRUNK_ID: " << serviceTrunk->id << "\n";
            }
            continue;
        }

        if (trunkSettings->dst_number_id > 0 && !repository.matchNumber(trunkSettings->dst_number_id, atoll(bNumber.c_str()))) {
            if (trace != nullptr) {
                *trace << "DEBUG|SERVICE TRUNK DECLINE|BY DST NUMBER MATCHING, SEVICE_TRUNK_ID: " << serviceTrunk->id << "\n";
            }
            continue;
        }

        pricelist = repository.getPricelist(trunkSettings->pricelist_id);
        if (pricelist == nullptr) {
            if (trace != nullptr) {
                *trace << "DEBUG|SERVICE TRUNK DECLINE|PRICELIST NOT FOUND BY ID: " << trunkSettings->pricelist_id << ", SEVICE_TRUNK_ID: " << serviceTrunk->id << "\n";
            }
            continue;
        }

        if (pricelist->local) {

            auto networkPrefix = repository.getNetworkPrefix(pricelist->local_network_config_id, atoll(bNumber.c_str()));
            if (networkPrefix == nullptr) {
                if (trace != nullptr) {
                    *trace << "DEBUG|SERVICE TRUNK DECLINE|NETWORK PREFIX NOT FOUND BY local_network_config_id: " << pricelist->local_network_config_id << ", PRICELIST_ID: " << pricelist->id << ", SEVICE_TRUNK_ID: " << serviceTrunk->id << "\n";
                }
                continue;
            }

            price = repository.getPrice(trunkSettings->pricelist_id, networkPrefix->network_type_id);
            if (price == nullptr) {
                if (trace != nullptr) {
                    *trace << "DEBUG|SERVICE TRUNK DECLINE|PRICE NOT FOUND: PRICELIST_ID: " << pricelist->id  << ", PREFIX: " << networkPrefix->network_type_id << ", SEVICE_TRUNK_ID: " << serviceTrunk->id << "\n";
                }
                continue;
            }


        } else {

            price = repository.getPrice(trunkSettings->pricelist_id, atoll(bNumber.c_str()));
            if (price == nullptr) {
                if (trace != nullptr) {
                    *trace << "DEBUG|SERVICE TRUNK DECLINE|PRICE NOT FOUND: PRICELIST_ID: " << pricelist->id  << ", PREFIX: " << atoll(bNumber.c_str()) << ", SEVICE_TRUNK_ID: " << serviceTrunk->id << "\n";
                }
                continue;
            }

        }

        if (trace != nullptr) {
            *trace << "DEBUG|SERVICE TRUNK ACCEPT|SEVICE_TRUNK_ID: " << serviceTrunk->id << "\n";
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
    double sumBalance2 = statsAccount2->getSumBalance(call.account_id, vat_rate) + call.cost;
    double sumDay2 = statsAccount2->getSumDay(call.account_id, vat_rate) + call.cost;

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
        
        // Блокировка МГМН если превышен лимит кредита
        if (!call.isLocal()  && client->isConsumedCreditLimit(spentBalanceSum)) {
            return "low_balance";
        }

        // Блокировка МГМН если превышен дневной лимит
        if (!call.isLocal() && client->isConsumedDailyLimit(spentDaySum)) {
            return "voip_disabled";
        }

        // Глобальная блокировка
        if (client->is_blocked) {
            return "voip_disabled";
        }

        // Блокировка МГМН
        if (!call.isLocal() && client->disabled) {
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
