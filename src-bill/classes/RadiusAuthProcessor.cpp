#include <vector>
#include "../common.h"
#include "Log.h"
#include "RadiusAuthProcessor.h"
#include "RadiusAuthServer.h"
#include "BillingCall.h"

void RadiusAuthProcessor::init() {

    if (!repository.prepare() || !repository.billingData->ready()) {
        billingNotReady = true;
        throw Exception("Billing not ready.", "RadiusAuthProcessor::init");
    }

    billingNotReady = false;

    int server_id = 0;


    if(app().conf.isApiHostMode() && request->region == 0) {
        server_id = repository.getServerIdByIP(request->nasIpAddress);
        request->region = server_id;

        if(server_id>0)
        {
            logRequest->params["use_secondary_router"] = 1;
            logRequest->params["from_region"] = server_id;
            Log::info("SecondaryRadiusAuthServer: from region#" + to_string(server_id));
        }
    }

    if(request->region>0) {

        server = repository.getServer(request->region);
        if(server) server_id = server->id;
    }

    origTrunk = repository.getTrunkByName(request->trunkName.c_str(),server_id);
    if (origTrunk == nullptr) {
        throw Exception("Udp request validation: trunk not found: " + request->trunkName,
                        "RadiusAuthProcessor::process");
    }


   if(request->region >0) {

        server = repository.getServer(request->region);
    }
     else {
        server = repository.getServer(origTrunk->server_id);
    }

    if (server == nullptr) {
        throw Exception(
                "Udp request validation: server_id" + to_string(origTrunk->server_id) + " by trunk not found: " +
                request->trunkName,
                "RadiusAuthProcessor::process");
    }

    if (trace != nullptr) {
        *trace << "INFO|REGION_ID|" << server->id << "\n";
    }

    aNumber = request->srcNumber;
    bNumber = request->dstNumber;

}

RadiusAuthProcessor::RadiusAuthProcessor(RadiusAuthRequest *request, RadiusAuthResponse *response,
                                         pLogMessage &logRequest) {
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

void RadiusAuthProcessor::process(std::map<int, std::pair<RejectReason, time_t> > *o_pAccountIdsBlockedBefore) {

    try {
        init();

        // Костыль для СОРМ-ирования в Москве.

        if ( ( app().conf.instance_id == 99 || this->request->region == 99 ) &&
                this->request->callingPartyCategory == "INTERCEPT") {
            response->setAccept();
            return;
        }

        //////

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
        StateMegaTrunk stateMegaTrunk(&repository);

        if(request->region > 0)
        {
            billingCall.setOrigRegion(request->region);
            stateMegaTrunk.setOrigRegion(request->region);
        }

        stateMegaTrunk.setTrace(trace);
        stateMegaTrunk.prepareFromCdr(&cdr,aNumber,bNumber); // Загружаем исходные данные для расчета МегаТранков из cdr- звонка.
        stateMegaTrunk.PhaseCalc(); // Расчет фаз маршутизации для Мегатранков

        Call call = Call(&cdr, CALL_ORIG);

        CallInfo callInfo;

        billingCall.calc(&call, &callInfo, &cdr, &stateMegaTrunk);

        account = callInfo.account;

        if (trace != nullptr) {
            *trace << "INFO|CALL|";
            call.dump(*trace);
            *trace << "\n";
        }


        string billResponse = analyzeCall(call, o_pAccountIdsBlockedBefore);
        if (trace != nullptr) {
            *trace << "INFO|BILL RESPONSE|" << billResponse << "\n";
        }

        prepareAuthLogReguestStage1(call, callInfo);

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

            outcomeId = processRouteTable(origTrunk->route_table_id, stateMegaTrunk);
            if (outcomeId == 0) {
                Log::warning("Route table: Outcome not found");
                response->setReleaseReason("NO_ROUTE_TO_DESTINATION");
                return;
            }
        }

        double buyRate = 0.0;
        Pricelist *firstBuyPricelist = 0;

        if (processOutcome(outcomeId, stateMegaTrunk, &buyRate, &firstBuyPricelist, callInfo.callIncomingFromMegatrunk)) {

            prepareAuthLogReguestStage2(call, callInfo, buyRate, firstBuyPricelist);

        }

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

    if ((origTrunk == nullptr) && !billingNotReady) {
        response->setReleaseReason("NO_ROUTE_TO_DESTINATION");
        return;
    }

    Log::warning("Fallback to default route");
    response->setAccept();
}

int RadiusAuthProcessor::processRouteTable(const int routeTableId, StateMegaTrunk &megaTrunk) {

    auto routeTable = repository.getRouteTable(routeTableId);
    if (routeTable == nullptr) {
        throw Exception("Route table #" + lexical_cast<string>(routeTableId) + " not found",
                        "RadiusAuthProcessor::processRouteTable");
    }

    Server *new_server = repository.getServer(routeTable->server_id);

    if( new_server != nullptr && server->id != new_server->id ) {
        if (trace != nullptr) {
            *trace << "INFO|CHANGE REGION| FROM #" << server->id << " TO #" << new_server->id << " " << "\n";
        }
        server = new_server;
    }

    if (trace != nullptr) {
        *trace << "INFO|ROUTE TABLE|" << routeTable->name << " (" << routeTableId << ") IN REGION #" << routeTable->server_id << "\n";
    }


    vector<RouteTableRoute *> routes;
    repository.getAllRouteTableRoutes(routes, routeTable->id);

    for (RouteTableRoute *route : routes) {

        if (route->a_number_id && !filterByNumber(route->a_number_id, aNumber)) {
            continue;
        }

        if (route->b_number_id && !filterByNumber(route->b_number_id, bNumber)) {
            continue;
        }

        if (route->outcome_id) {

            auto outcome = repository.getOutcome(route->outcome_id);

            if (outcome != nullptr && outcome->isMegToReg() && !megaTrunk.isMegaTrunkPhase1())
                continue;

            if (outcome != nullptr && outcome->isMegToMeg() && !megaTrunk.isMegaTrunkPhase2())
                continue;

            return route->outcome_id;
        }

        if (route->outcome_route_table_id) {
            int outcomeId = processRouteTable(route->outcome_route_table_id, megaTrunk);
            if (outcomeId != 0) {
                return outcomeId;
            }
        }
    }

    return 0;
}

// Возвращает true, если в pBuyRate возвращается себестоимость одной минуты звонка на транк.
bool RadiusAuthProcessor::processOutcome(int outcomeId, StateMegaTrunk &stateMegaTrunk, double *pBuyRate,
                                         Pricelist **pFirstBuyPricelist, bool skipLoopProtection) {
    if (pBuyRate) {
        *pBuyRate = 0;
    }

    if (pFirstBuyPricelist) {
        *pFirstBuyPricelist = nullptr;
    }

    auto outcome = repository.getOutcome(outcomeId);
    if (outcome == nullptr) {
        throw Exception("Outcome #" + lexical_cast<string>(outcomeId) + " not found",
                        "RadiusAuthProcessor::processOutcome");
    }

    if (trace != nullptr) {
        *trace << "INFO|OUTCOME|" << outcome->name << " (" << outcomeId << ")" << "\n";
    }

    if (outcome->isAuto()) {

        return processAutoOutcome(pBuyRate, pFirstBuyPricelist, skipLoopProtection);

    } else if (outcome->isMegToReg()) {

        processMegaTrunkPhase1(stateMegaTrunk);
        return false;

    } else if (outcome->isMegToMeg()) {

        processMegaTrunkPhase2(stateMegaTrunk);
        return false;

    } else if (outcome->isRouteCase()) {

        processRouteCaseOutcome(outcome);
        return false;

    } else if (outcome->isReleaseReason()) {

        processReleaseReasonOutcome(outcome);
        return false;

    } else if (outcome->isAirp()) {

        processAirpOutcome(outcome);
        return false;

    } else if (outcome->isAccept()) {

        response->setAccept();
        return false;

    }

    throw Exception("Unexpected type of outcome #" + lexical_cast<string>(outcome->id),
                    "RadiusAuthProcessor::processOutcome");
}

// Возвращает true, если есть хотя бы один транк,
// а в pBuyRate возвращается себестоимость одной минуты звонка на этот транк.
bool RadiusAuthProcessor::processAutoOutcome(double *pBuyRate, Pricelist **pFirstBuyPricelist, bool skipLoopProtection) {

    if (pBuyRate) {
        *pBuyRate = 0;
    }

    if (pFirstBuyPricelist) {
        *pFirstBuyPricelist = nullptr;
    }

    ServiceTrunkOrder origServiceTrunkOrder;

    getAvailableOrigServiceTrunk(origServiceTrunkOrder);

    bool fUseMinimalki = false;

    if (this->origTrunk != nullptr) fUseMinimalki = this->origTrunk->sw_minimalki;

    vector<ServiceTrunkOrder> termServiceTrunks;

    getAvailableTermServiceTrunk(termServiceTrunks, origServiceTrunkOrder, fUseMinimalki, skipLoopProtection);

    double origRub = origServiceTrunkOrder.is_price_present() ? this->repository.priceToRoubles(
            origServiceTrunkOrder.getPrice(), origServiceTrunkOrder.getCurrency()) : 0;

    return processAutoRouteResponse(termServiceTrunks, pBuyRate, pFirstBuyPricelist,
                                    origRub);
}


void RadiusAuthProcessor::getAvailableOrigServiceTrunk(ServiceTrunkOrder &origServiceTrunkOrder) {

    vector<ServiceTrunkOrder> trunkSettingsOrderList;

    repository.getTrunkSettingsOrderList(trunkSettingsOrderList, origTrunk, atoll(aNumber.c_str()),
                                         atoll(bNumber.c_str()), SERVICE_TRUNK_SETTINGS_ORIGINATION);

    repository.orderOrigTrunkSettingsOrderList(trunkSettingsOrderList);

    if (trunkSettingsOrderList.size() > 0) {
        origServiceTrunkOrder = trunkSettingsOrderList.at(0);
    }
}

void RadiusAuthProcessor::getAvailableTermServiceTrunk(vector<ServiceTrunkOrder> &termServiceTrunks,
                                                       ServiceTrunkOrder &origServiceTrunkOrder,
                                                       bool fUseMinimalki, bool skipLoopProtection ) {
    vector<Trunk *> termTrunks;

    set<int> simblingRegions;

    int server_id = app().conf.instance_id;

    if (server != nullptr) server_id = server->id;

    repository.getSimblingRegion(simblingRegions, server_id);

    repository.getAllAutoRoutingTrunks(termTrunks, server_id,simblingRegions);

    if (trace != nullptr) {
        *trace << "INFO| USE_MINIMALKI |  " << (fUseMinimalki ? "YES" : "NO") << "" << "\n";
    }


    for (auto termTrunk : termTrunks) {

        if (this->origTrunk != nullptr && this->origTrunk->id == termTrunk->id && !skipLoopProtection) {
            if (trace != nullptr) {
                *trace << "INFO|TERM SERVICE TRUNK DECLINE|SKIP LOOP ROUTE, " << termTrunk->name << " (" <<
                       termTrunk->id << ")" << "\n";
            }
            continue;

        }

        if (!autoTrunkFilterSrcTrunk(termTrunk)) {
            if (trace != nullptr) {
                *trace << "INFO|TERM SERVICE TRUNK DECLINE|BY TRUNK FILTER, " << termTrunk->name << " (" <<
                       termTrunk->id << ")" << "\n";
            }
            continue;
        }
        if (!autoTrunkFilterSrcNumber(termTrunk)) {
            if (trace != nullptr) {
                *trace << "INFO|TERM SERVICE TRUNK DECLINE|BY SRC NUMBER FILTER, " << termTrunk->name << " (" <<
                       termTrunk->id << ")" << "\n";
            }
            continue;
        }
        if (!autoTrunkFilterDstNumber(termTrunk)) {
            if (trace != nullptr) {
                *trace << "INFO|TERM SERVICE TRUNK DECLINE|BY DST NUMBER FILTER, " << termTrunk->name << " (" <<
                       termTrunk->id << ")" << "\n";
            }
            continue;
        }

        if (origTrunk->orig_redirect_number && request->redirectNumber.size() > 0 && !termTrunk->term_redirect_number) {
            if (trace != nullptr) {
                *trace << "INFO|TERM SERVICE TRUNK DECLINE|CAUSE NOT SUPPORT REDIRECTING NUMBER, " << termTrunk->name <<
                       " (" << termTrunk->id << ")" << "\n";
            }
            continue;
        }

        vector<ServiceTrunkOrder> trunkSettingsOrderList;

        repository.getTrunkSettingsOrderList(trunkSettingsOrderList, termTrunk, atoll(aNumber.c_str()),
                                             atoll(bNumber.c_str()), SERVICE_TRUNK_SETTINGS_TERMINATION);

        repository.orderTermTrunkSettingsOrderList(trunkSettingsOrderList, fUseMinimalki, time(nullptr));

        for (auto termOrder : trunkSettingsOrderList) {
            if (
                    server->min_price_for_autorouting > 0
                    && termOrder.price && termOrder.pricelist
                    && this->repository.priceToRoubles(termOrder.price->price, *termOrder.pricelist) >
                       server->min_price_for_autorouting
                    && account != nullptr
                    && !account->anti_fraud_disabled) {
                if (trace != nullptr) {
                    *trace << "INFO|TERM TRUNK SETTINGS DECLINE|CAUSE ANTI FRAUD: " << termTrunk->name << " (" <<
                           termTrunk->id << ")" << ", SERVICE TRUNK ID: " << termOrder.serviceTrunk->id <<
                           "TRUNK SETTINGS ID: " << termOrder.trunkSettings->id << " / "
                           << termOrder.trunkSettings->order <<
                           "\n";
                }
            } else {
                auto origSettings = origServiceTrunkOrder.trunkSettings;

                if (origSettings && origSettings->minimum_margin_type != SERVICE_TRUNK_SETTINGS_MIN_MARGIN_ABSENT
                    && termOrder.is_price_present() && origServiceTrunkOrder.is_price_present()) {

                    if (origSettings->minimum_margin_type == SERVICE_TRUNK_SETTINGS_MIN_MARGIN_PERCENT
                        || origSettings->minimum_margin_type == SERVICE_TRUNK_SETTINGS_MIN_MARGIN_VALUE) {

                        double origRub = this->repository.priceToRoubles(origServiceTrunkOrder.getPrice(),
                                                                         origServiceTrunkOrder.getCurrency());
                        double termRub = this->repository.priceToRoubles(termOrder.getPrice(), termOrder.getCurrency());

                        if (origRub > 0.000001 && termRub > 0.000001) {

                            double profit = origRub - termRub;

                            // Под маржой в процентах здесь понимается НАЦЕНКА
                            // Под маржой в абс. единицах - ПРИБЫЛЬ
                            double trunkMargin =
                                    origSettings->minimum_margin_type == SERVICE_TRUNK_SETTINGS_MIN_MARGIN_VALUE
                                    ? profit
                                    : profit / termRub * 100;

                            if (trunkMargin < origSettings->minimum_margin) {
                                if (trace != nullptr) {
                                    *trace << "INFO|TRUNK MARGIN DECLINE|CAUSE MIN MARGIN: " << termTrunk->name <<
                                           " (" << termTrunk->id << ")" << ", SERVICE TRUNK ID: "
                                           << termOrder.serviceTrunk->id
                                           << ", MIN MARGIN: " << origSettings->minimum_margin << ", TRUNK MARGIN: " <<
                                           trunkMargin << ", ORIGRUB:" << origRub << ", TERMRUB:" << termRub << "\n";
                                }

                                continue;
                            } else {
                                if (trace != nullptr) {
                                    *trace << "INFO|TRUNK MARGIN ACCEPT|CAUSE MIN MARGIN: " << termTrunk->name <<
                                           " (" << termTrunk->id << ")" << ", SERVICE TRUNK ID: "
                                           << termOrder.serviceTrunk->id
                                           << ", MIN MARGIN: " << origSettings->minimum_margin << ", TRUNK MARGIN: " <<
                                           trunkMargin << ", ORIGRUB:" << origRub << ", TERMRUB:" << termRub << "\n";
                                }
                            }
                        }
                    } else {
                        if (trace != nullptr) {
                            *trace << "DEBUG|TERM SERVICE TRUNK ACCEPT|UNEXPECTED MIN MARGIN TYPE: " <<
                                   termTrunk->name << " (" << termTrunk->id << ")" << ", SERVICE TRUNK ID: "
                                   << termOrder.serviceTrunk->id << ", STRANGE MIN MARGIN TYPE: " <<
                                   origSettings->minimum_margin_type << "\n";
                        }
                    }
                }

                termServiceTrunks.push_back(termOrder);
                if (trace != nullptr) {
                    *trace << "INFO|TERM TRUNK SETTINGS ACCEPT|" << termTrunk->name << " (" << termTrunk->id << ")" <<
                           ", SERVICE TRUNK ID: " << termOrder.serviceTrunk->id << "TRUNK SETTINGS ID: " <<
                           termOrder.trunkSettings->id << " / " << termOrder.trunkSettings->order << "\n";
                }
            }
        }
    }

    for (auto termServiceTrunk = termServiceTrunks.begin();
         termServiceTrunk != termServiceTrunks.end(); termServiceTrunk++) { // Расчитываем приоритеты транков
        if (termServiceTrunk->trunk != nullptr) {
            int t = termServiceTrunk->trunk->default_priority;
            termServiceTrunk->priority = t;
            int trunk_id = termServiceTrunk->trunk->id;
            vector<TrunkPriority> trunkPriorityList;
            repository.getTrunkPriority(trunk_id, trunkPriorityList);
            for (auto trunkPriority : trunkPriorityList) {

                int num_a = trunkPriority.number_id_filter_a;
                bool f_a = true;
                int num_b = trunkPriority.number_id_filter_b;
                bool f_b = true;

                if (num_a > 0) f_a = filterByNumber(trunkPriority.number_id_filter_a, aNumber);
                if (num_b > 0) f_b = filterByNumber(trunkPriority.number_id_filter_b, bNumber);

                if (f_a && f_b && matchTrunkGroup(trunkPriority.trunk_group_id, origTrunk->id)) {
                    termServiceTrunk->priority = trunkPriority.priority;
                    break;
                }

            }
        }
    }

    repository.orderTermTrunkSettingsOrderList(termServiceTrunks, fUseMinimalki, time(nullptr));
}

bool RadiusAuthProcessor::processAutoRouteResponse(vector<ServiceTrunkOrder> &termOrders, double *pBuyRate,
                                                   Pricelist **pFirstBuyPricelist, double origRub) {

    if (pBuyRate) {
        *pBuyRate = 0;
    }

    if (pFirstBuyPricelist) {
        *pFirstBuyPricelist = nullptr;
    }

    bool hasRateForATrunk = false;

    if (termOrders.size() == 0) {
        Log::warning("Auto Route not contains operators");
        response->setReleaseReason("NO_ROUTE_TO_DESTINATION");
        return hasRateForATrunk;
    }

    if (trace != nullptr) {
        *trace << "INFO|TERM SERVICE TRUNKS|" << "\n";
    }

    for (ServiceTrunkOrder &trunkOrder : termOrders) {
        if (!hasRateForATrunk) {
            hasRateForATrunk = true;
            if (pBuyRate) {
                *pBuyRate = 0;
                if(trunkOrder.account!= nullptr){
                    if(trunkOrder.account->account_version==4 && trunkOrder.price != nullptr )
                        *pBuyRate = trunkOrder.price->price;
                    if(trunkOrder.account->account_version==5)
                        *pBuyRate = trunkOrder.nnp_price;
                }
            }

            if (pFirstBuyPricelist) {
                *pFirstBuyPricelist = trunkOrder.pricelist;
            }
        }

        if (trace != nullptr) {
            *trace << "INFO||PRICE: " << trunkOrder.price->price;
            *trace << ", TRUNK: " << trunkOrder.trunk->name << " (" << trunkOrder.trunk->id << ")";
            *trace << ", PRIORITY: " << trunkOrder.priority;
            *trace << ", SERVICE TRUNK " << trunkOrder.serviceTrunk->id;
            if (trunkOrder.pricelist) {
                *trace << ", PRICELIST: " << trunkOrder.pricelist->id;
                *trace << ", PRICELIST CURRENCY: " << trunkOrder.pricelist->currency_id;
            }
            if (trunkOrder.price) {
                *trace << ", PREFIX: " << trunkOrder.price->prefix;
            }
            if (trunkOrder.nnpPackage) {
                *trace << ", NNPPACKAGE_ID: " << trunkOrder.nnpPackage->id;
                *trace << ", NNPPACKAGE_CURRENCY: " << trunkOrder.nnpPackage->currency_id;
                *trace << ", NNP_PRICE: " << trunkOrder.nnp_price;
            }
            if (trunkOrder.nnpPackagePrice) {
                *trace << ", NNPPACKAGEPRICE_ID: " << trunkOrder.nnpPackagePrice->id;
            }
            if (trunkOrder.nnpPackagePricelist) {
                *trace << ", NNPPACKAGEPRICELIST_ID: " << trunkOrder.nnpPackagePricelist->id;
            }


            if (trunkOrder.trunkSettings != nullptr && trunkOrder.statsTrunkSettings != nullptr) {
                if (trunkOrder.trunkSettings->minimum_cost > 0) {
                    *trace << ", MINIMUM_COST: " << trunkOrder.trunkSettings->minimum_cost;
                    *trace << ", USED_CREDIT: " << trunkOrder.statsTrunkSettings->used_credit;
                }
                if (trunkOrder.trunkSettings->minimum_minutes > 0) {
                    *trace << ", MINIMUM_MINUTES: " << trunkOrder.trunkSettings->minimum_minutes
                           << " ( " << trunkOrder.trunkSettings->minimum_minutes * 60 << " seconds )";
                    *trace << ", USED_SECONDS: " << trunkOrder.statsTrunkSettings->used_seconds;
                }
                if (trunkOrder.trunkSettings->minimum_margin_type != SERVICE_TRUNK_SETTINGS_MIN_MARGIN_ABSENT) {

                    *trace << ", MINIMUM_MARGIN_TYPE:" << trunkOrder.trunkSettings->minimum_margin_type <<
                           ", MINIMUM_MARGIN:" << trunkOrder.trunkSettings->minimum_margin;
                }
            }
            *trace << ", TERM PRICE: " << trunkOrder.price->price;
            *trace << ", ORIG PRICE: " << origRub;
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

        if (routeCase.length()) {
            routeCase += ",";
        }

        routeCase += trunkOrder.trunk->trunk_name;

        // Это ограничение признано разумным из-за обычно большого времени на попытки
        if (trunkIds.size() == 3) {
            break;
        }
    }

    response->setRouteCase(routeCase);

    return hasRateForATrunk;
}

void RadiusAuthProcessor::processRouteCaseOutcome(Outcome *outcome) {
    auto routeCase = repository.getRouteCase(outcome->route_case_id);
    auto routeCase1 = repository.getRouteCase(outcome->route_case_1_id);
    auto routeCase2 = repository.getRouteCase(outcome->route_case_2_id);


    if (routeCase == nullptr) {
        throw Exception("Route case #" + lexical_cast<string>(outcome->route_case_id) + " not found",
                        "RadiusAuthProcessor::processRouteCaseOutcome");
    }

    string sRouteCase = routeCase->name;

    if (routeCase1 != nullptr) sRouteCase += string(",") + routeCase1->name;
    if (routeCase2 != nullptr) sRouteCase += string(",") + routeCase2->name;

    response->setRouteCase(sRouteCase);

    if (outcome->calling_station_id[0] != 0) {
        response->srcNumber = outcome->calling_station_id;
    }
    if (outcome->called_station_id[0] != 0) {
        response->dstNumber = outcome->called_station_id;
    }
}

void RadiusAuthProcessor::processReleaseReasonOutcome(Outcome *outcome) {
    auto releaseReason = repository.getReleaseReason(outcome->release_reason_id);
    if (releaseReason == nullptr) {
        throw Exception("Release reason #" + lexical_cast<string>(outcome->release_reason_id) + " not found",
                        "RadiusAuthProcessor::processReleaseReasonOutcome");
    }

    response->setReleaseReason(releaseReason->name);
}

void RadiusAuthProcessor::processAirpOutcome(Outcome *outcome) {
    auto airp = repository.getAirp(outcome->airp_id);
    if (airp == nullptr) {
        throw Exception("Airp #" + lexical_cast<string>(outcome->airp_id) + " not found",
                        "RadiusAuthProcessor::processAirpOutcome");
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
        throw Exception("Number #" + lexical_cast<string>(numberId) + " not found",
                        "RadiusAuthProcessor::filterByNumber");
    }

    auto prefixlistIds = number->getPrefixlistIds();
    for (auto it = prefixlistIds.begin(); it != prefixlistIds.end(); ++it) {

        auto prefixlist = repository.getPrefixlist(*it);
        if (prefixlist == nullptr) {
            throw Exception("Prefixlist #" + lexical_cast<string>(*it) + " not found",
                            "RadiusAuthProcessor::filterByNumber");
        }

        auto prefix = repository.getPrefixlistPrefix(prefixlist->id, strNumber.c_str());
        if (prefix) {
            if (trace != nullptr) {
                *trace << "DEBUG|PREFIXLIST MATCHED|" << strNumber << " in " << prefixlist->name << " (" <<
                       prefixlist->id << ")" << "\n";
            }
            if (trace != nullptr) {
                *trace << "INFO|NUMBER MATCHED|" << strNumber << " in " << number->name << " (" << numberId << ")" <<
                       "\n";
            }
            return true;
        }

        if (trace != nullptr) {
            *trace << "DEBUG|PREFIXLIST NOT MATCHED|" << strNumber << " not in " << prefixlist->name << " (" <<
                   prefixlist->id << ")" << "\n";
        }
    }

    if (trace != nullptr) {
        *trace << "INFO|NUMBER NOT MATCHED|" << strNumber << " not in " << number->name << " (" << numberId << ")" <<
               "\n";
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
    if (!origTrunk->our_trunk
        || !origTrunk->auth_by_number
        || !server->calling_station_id_for_line_without_number[0] != 0
            ) {
        return;
    }

    bool lineWithoutNumber = false;

    if (aNumber.find("*") != string::npos) {
        lineWithoutNumber = true;
    }

    if (aNumber.find("+") != string::npos) {
        lineWithoutNumber = true;
    }

    if (lineWithoutNumber) {
        if (origTrunk->orig_redirect_number && request->redirectNumber.size() > 0) {
            return;
        }

        response->srcNumber = server->calling_station_id_for_line_without_number;
        if (trace != nullptr) {
            *trace << "INFO|LINE WITH NUMBER|SET RESPONSE.CALLING = " << response->srcNumber << "\n";
        }
    }
}

bool RadiusAuthProcessor::autoTrunkFilterSrcTrunk(Trunk *termTrunk) {
    vector<TrunkTrunkRule> resultTrunkGroupRules;
    repository.getAllTrunkGroupRules(resultTrunkGroupRules, termTrunk->id);

    if (termTrunk->source_trunk_rule_default_allowed) {
        for (int i = 0; i < resultTrunkGroupRules.size(); i++) {

            int num_a = resultTrunkGroupRules[i].number_id_filter_a;
            bool f_a = true;
            int num_b = resultTrunkGroupRules[i].number_id_filter_b;
            bool f_b = true;

            int trunk_group = resultTrunkGroupRules[i].trunk_group_id;
            bool f_trunk_group = true;

            if (num_a > 0) f_a = filterByNumber(num_a, aNumber);
            if (num_b > 0) f_b = filterByNumber(num_b, bNumber);
            if (origTrunk != nullptr && trunk_group > 0) f_trunk_group = matchTrunkGroup(trunk_group, origTrunk->id);

            if (f_a && f_b && f_trunk_group) {
                return false;
            }
        }
        return true;
    } else {
        for (int i = 0; i < resultTrunkGroupRules.size(); i++) {

            int num_a = resultTrunkGroupRules[i].number_id_filter_a;
            bool f_a = true;
            int num_b = resultTrunkGroupRules[i].number_id_filter_b;
            bool f_b = true;

            if (num_a > 0) f_a = filterByNumber(resultTrunkGroupRules[i].number_id_filter_a, aNumber);
            if (num_b > 0) f_b = filterByNumber(resultTrunkGroupRules[i].number_id_filter_b, bNumber);

            if (f_a && f_b && matchTrunkGroup(resultTrunkGroupRules[i].trunk_group_id, origTrunk->id)) {
                return true;
            }
        }
        return false;
    }
}

bool RadiusAuthProcessor::autoTrunkFilterSrcNumber(Trunk *termTrunk) {
    vector<TrunkRule *> rules;
    repository.getAllTrunkRules(rules, termTrunk->id, false);
    if (termTrunk->source_rule_default_allowed) {
        for (TrunkRule *rule : rules) {
            if (matchPrefixlist(rule->prefixlist_id, aNumber)) {
                return false;
            }
        }
        return true;
    } else {
        for (TrunkRule *rule : rules) {
            if (matchPrefixlist(rule->prefixlist_id, aNumber)) {
                return true;
            }
        }
        return false;
    }
}

bool RadiusAuthProcessor::autoTrunkFilterDstNumber(Trunk *termTrunk) {
    vector<TrunkRule *> rules;
    repository.getAllTrunkRules(rules, termTrunk->id, true);
    if (termTrunk->destination_rule_default_allowed) {
        for (TrunkRule *rule : rules) {
            if (matchPrefixlist(rule->prefixlist_id, bNumber)) {
                return false;
            }
        }
        return true;
    } else {
        for (TrunkRule *rule : rules) {
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
        throw Exception("TrunkGroup #" + lexical_cast<string>(trunkGroupId) + " not found",
                        "RadiusAuthProcessor::matchTrunkGroup");
    }

    vector<int> trunkIds;
    repository.getAllTrunkGroupTrunkIds(trunkIds, trunkGroupId);
    for (int trunkId : trunkIds) {
        if (trunkId == matchTrunkId) {
            if (trace != nullptr) {
                *trace << "DEBUG|TRUNK GROUP MATCHED|" << trunkGroup->name << "(" << trunkGroup->id << "): " <<
                       matchTrunkId << " in ";
                for (int tmpId : trunkIds) *trace << tmpId << ",";
                *trace << "\n";
            }
            return true;
        }
    }

    if (trace != nullptr) {
        *trace << "DEBUG|TRUNK GROUP NOT MATCHED|" << trunkGroup->name << "(" << trunkGroup->id << "): " <<
               matchTrunkId << " not in ";
        for (int tmpId : trunkIds) *trace << tmpId << ",";
        *trace << "\n";
    }
    return false;
}

bool RadiusAuthProcessor::matchPrefixlist(const int prefixlistId, string strNumber) {
    auto prefixlist = repository.getPrefixlist(prefixlistId);
    if (prefixlist == nullptr) {
        throw Exception("Prefixlist #" + lexical_cast<string>(prefixlistId) + " not found",
                        "RadiusAuthProcessor::matchPrefixlist");
    }

    auto prefix = repository.getPrefixlistPrefix(prefixlist->id, strNumber.c_str());
    return prefix != nullptr;
}

string RadiusAuthProcessor::analyzeCall(Call &call,
                                        std::map<int, std::pair<RejectReason, time_t> > *o_pAccountIdsBlockedBefore) {

    if (origTrunk != nullptr && ( origTrunk->tech_trunk || origTrunk->pstn_trunk )) {
        return "accept";
    }

    if (repository.isEmergencyCall(bNumber, origTrunk->server_id)) {
        return "accept";
    }

    if (call.account_id == 0) {
        return "reject";
    }

    auto client = repository.getAccount(call.account_id);
    if (client == nullptr) {
        return "reject";
    }

    if (o_pAccountIdsBlockedBefore->count(call.account_id) > 0) {

        // какой-то из параллельных звонков уже сделал баланс отрицательным
        std::pair<RejectReason, time_t> block = (*o_pAccountIdsBlockedBefore)[call.account_id];
        return block.first == REASON_NO_BALANCE ? "low_balance" : "voip_disabled";
    }

    double vat_rate = repository.getVatRate(client);

    double sumBalance = repository.billingData->statsAccountGetSumBalance(call.account_id, vat_rate);
    double sumDay = repository.billingData->statsAccountGetSumDay(call.account_id, vat_rate);
    double sumMNDay = repository.billingData->statsAccountGetSumMNDay(call.account_id, vat_rate);

    auto statsAccount2 = repository.currentCalls->getStatsAccount().get();
    double sumBalance2 = statsAccount2->getSumBalance(call.account_id, vat_rate) + call.cost;
    double sumDay2 = statsAccount2->getSumDay(call.account_id, vat_rate) + call.cost;
    double sumMNDay2 = statsAccount2->getSumMNDay(call.account_id, vat_rate) + call.cost;

    double globalBalanceSum, globalDaySum, globalDayMNSum;
    fetchGlobalCounters(call.account_id, globalBalanceSum, globalDaySum, globalDayMNSum, vat_rate);

    double spentBalanceSum, spentDaySum, spentDayMNSum;
    spentBalanceSum = sumBalance + sumBalance2 + globalBalanceSum;
    spentDaySum = sumDay + sumDay2 + globalDaySum;
    spentDayMNSum = sumMNDay + sumMNDay2 + globalDayMNSum;

    InstanceSettings *instanceSettings = repository.getInstanceSettings(app().conf.instance_id);
    bool auto_lock_finance = (instanceSettings!= nullptr)? instanceSettings->auto_lock_finance:false;

    if (call.trunk_service_id != 0) {

        if (isLowBalance(&Client::isConsumedCreditLimit, REASON_NO_BALANCE, client, spentBalanceSum, call,
                         o_pAccountIdsBlockedBefore) && auto_lock_finance) {

            // не можем говорить ни секунды
            return "low_balance";
        }

    } else if (call.number_service_id != 0 && call.orig) {

        // Блокировка МГМН если превышен лимит кредита
        if (isLowBalance(&Client::isConsumedCreditLimit, REASON_NO_BALANCE, client, spentBalanceSum, call,
                         o_pAccountIdsBlockedBefore)) {
            // Если звонок платный
            if (abs(call.cost) > 0.000001 && auto_lock_finance ) {
                return "low_balance";
            }
        }

        // Блокировка МГМН если превышен дневной лимит
        if (!call.isLocal() &&
            isLowBalance(&Client::isConsumedDailyLimit, REASON_DAILY_LIMIT, client, spentDaySum, call,
                         o_pAccountIdsBlockedBefore)) {
            return "voip_disabled";
        }

        // Блокировка МН если превышен дневной лимит на МН звонки
        if (call.isInternational() &&
            isLowBalance(&Client::isConsumedDailyMNLimit, REASON_DAILY_LIMIT, client, spentDayMNSum, call,
                         o_pAccountIdsBlockedBefore)) {
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


bool RadiusAuthProcessor::isLowBalance(bool (Client::*checkLimit)(double), RejectReason reason, Client *client,
                                       double spentBalanceSum, Call &call,
                                       std::map<int, std::pair<RejectReason, time_t> > *o_pAccountIdsBlockedBefore) {
    if ((client->*checkLimit)(spentBalanceSum)) {

        if ((client->*checkLimit)(spentBalanceSum - call.cost)) {

            // не можем говорить ни секунды
            return true;

        } else {

            // можем говорить меньше минуты, блокируем попытки параллельных звонков
            (*o_pAccountIdsBlockedBefore)[call.account_id] = std::make_pair(reason, time(0) + 60);
        }
    }
    return false;
}

void RadiusAuthProcessor::fetchGlobalCounters(int accountId, double &globalBalanceSum, double &globalDaySum,
                                              double &globalDayMNSum, double vat_rate) {

    GlobalCounters *globalCounter = nullptr;
    if (repository.data->globalCounters.ready()) {
        globalCounter = repository.data->globalCounters.get()->find(accountId);
    }

    if (globalCounter != nullptr) {
        globalBalanceSum = globalCounter->sumBalance(vat_rate);
        globalDaySum = globalCounter->sumDay(vat_rate);
        globalDayMNSum = globalCounter->sumMNDay(vat_rate);
    } else {
        globalBalanceSum = 0.0;
        globalDaySum = 0.0;
        globalDayMNSum = 0.0;
    }
}

void RadiusAuthProcessor::prepareAuthLogReguestStage1(Call &call, CallInfo &callInfo) {

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
    logRequest->params["tech_trunk"] = (origTrunk!=nullptr && origTrunk->tech_trunk)? "true" : "false";
    logRequest->params["pstn_trunk"] = (origTrunk!=nullptr && origTrunk->pstn_trunk)? "true" : "false";

    if (callInfo.pricelist && callInfo.pricelist->currency_id[0]) {
        logRequest->params["rate_currency"] = callInfo.pricelist->currency_id;
        if (trace != nullptr) {
            *trace << "INFO|SELL PRICELIST CURRENCY: " << callInfo.pricelist->currency_id << "\n";
        }
    }

    if (callInfo.account != nullptr) {
        double vat_rate = repository.getVatRate(callInfo.account);

        double sumBalance = repository.billingData->statsAccountGetSumBalance(callInfo.account->id, vat_rate);
        double sumDay = repository.billingData->statsAccountGetSumDay(callInfo.account->id, vat_rate);
        double sumMNDay = repository.billingData->statsAccountGetSumMNDay(callInfo.account->id, vat_rate);

        auto statsAccount2 = repository.currentCalls->getStatsAccount().get();
        double sumBalance2 = statsAccount2->getSumBalance(callInfo.account->id, vat_rate) + call.cost;
        double sumDay2 = statsAccount2->getSumDay(callInfo.account->id, vat_rate) + call.cost;
        double sumMNDay2 = statsAccount2->getSumMNDay(callInfo.account->id, vat_rate) + call.cost;

        double globalBalanceSum, globalDaySum, globalDayMNSum;
        fetchGlobalCounters(callInfo.account->id, globalBalanceSum, globalDaySum, globalDayMNSum, vat_rate);

        double spentBalanceSum, spentDaySum, spentDayMNSum;
        spentBalanceSum = sumBalance + sumBalance2 + globalBalanceSum;
        spentDaySum = sumDay + sumDay2 + globalDaySum;
        spentDayMNSum = sumMNDay + sumMNDay2 + globalDayMNSum;


        logRequest->params["balance_stat"] = callInfo.account->balance;
        logRequest->params["balance_local"] = sumBalance;
        logRequest->params["balance_current"] = sumBalance2;
        logRequest->params["balance_global"] = globalBalanceSum;
        logRequest->params["balance_realtime"] = callInfo.account->balance + spentBalanceSum;
        if (callInfo.account->hasCreditLimit()) {
            logRequest->params["credit_limit"] = callInfo.account->credit;
            logRequest->params["credit_available"] =
                    callInfo.account->balance + callInfo.account->credit + spentBalanceSum;
        }

        logRequest->params["daily_local"] = sumDay;
        logRequest->params["daily_current"] = sumDay2;
        logRequest->params["daily_global"] = globalDaySum;
        logRequest->params["daily_total"] = spentDaySum;
        if (callInfo.account->hasDailyLimit()) {
            logRequest->params["daily_limit"] = callInfo.account->limit_d;
            logRequest->params["daily_available"] = callInfo.account->limit_d + spentDaySum;
        }
        if (callInfo.account->hasDailyMNLimit()) {
            logRequest->params["daily_mn_limit"] = callInfo.account->limit_d_mn;
            logRequest->params["daily_mn_available"] = callInfo.account->limit_d_mn + spentDayMNSum;
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

}

void RadiusAuthProcessor::prepareAuthLogReguestStage2(Call &call, CallInfo &callInfo, double buyRate,
                                                      Pricelist *firstBuyPricelist) {

// Логируем себестоимость минуты звонка
    logRequest->params["rate_buy"] = buyRate;

    if (firstBuyPricelist && firstBuyPricelist->currency_id[0]) {
        logRequest->params["rate_buy_currency"] = firstBuyPricelist->currency_id;
        if (trace != nullptr) {
            *trace << "INFO|BUY PRICELIST CURRENCY: " << firstBuyPricelist->currency_id << "\n";
        }

        if (callInfo.pricelist && callInfo.pricelist->currency_id[0]) {

            double buyPriceRub = repository.priceToRoubles(buyRate, *firstBuyPricelist);
            double sellPriceRub = repository.priceToRoubles(call.rate, *callInfo.pricelist);

            logRequest->params["rate_buy_rub"] = buyPriceRub;
            logRequest->params["rate_rub"] = sellPriceRub;

            double profit = sellPriceRub - buyPriceRub;

            if (abs(call.rate) > 0.000001 && abs(buyRate) > 0.000001) {
                logRequest->params["profit_per_minute"] = profit;
                logRequest->params["profit_markup_per_minute"] = profit / buyPriceRub * 100.0;
            }
        }
    }

}


void RadiusAuthProcessor::processMegaTrunkPhase1(StateMegaTrunk &megaTrunk) {

    int regionNum = megaTrunk.getDestinationRegion(); // Получаем номер региона, куда нужно направить звонок

    string routeCase;
    vector<Trunk *> resultTrunks;
    set<int> simblingRegions;

    int server_id = app().conf.instance_id;
    if (server != nullptr) server_id = server->id;
    repository.getSimblingRegion(simblingRegions, server_id);

    repository.getAllRoadToRegion(resultTrunks, server_id, regionNum, simblingRegions, trace);

    std::random_shuffle(resultTrunks.begin(), resultTrunks.end());

    for (Trunk *trunk : resultTrunks) {
        if (!routeCase.empty()) routeCase += ',';
        routeCase += trunk->trunk_name;
    }

    if (routeCase.empty()) {
        throw Exception("not found road to Region #" + lexical_cast<string>(regionNum),
                        "RadiusAuthProcessor::processMegaTrunkPhase1");
    }

    if (trace != nullptr) {
        *trace << "INFO|MEGATRUNK|PHASE 1: Transfer to region #" << regionNum;
        *trace << "\n";
    }

    response->setRouteCase(routeCase);

}

void RadiusAuthProcessor::processMegaTrunkPhase2(StateMegaTrunk &megaTrunk) {

    Trunk *trunk = megaTrunk.getDestinationMegaTrunk(); // Получаем Мегатранк, в котором нужно собрать звонки.

    if (trunk == nullptr) {
        throw Exception("not found MegaTrunk",
                        "RadiusAuthProcessor::processMegaTrunkPhase1");
    }

    if (trace != nullptr) {
        *trace << "INFO|MEGATRUNK|PHASE 2: Transfer to MegaTrunk [" << trunk->trunk_name << "]";
        *trace << "\n";
    }

    response->setRouteCase(trunk->trunk_name);

}
