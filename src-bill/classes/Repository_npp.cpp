#include "Repository.h"
#include "RadiusAuthProcessor.h"
#include "BillingCall.h"


void
Repository::getNNPTrunkSettingsOrderList(vector<ServiceTrunkOrder> &resultTrunkSettingsTrunkOrderList, Trunk *trunk,
                                         long long int srcNumber, long long int dstNumber, set<int> &nnpDestinationIds,
                                         int destinationType) {
    vector<ServiceTrunk *> serviceTrunks;
    getAllServiceTrunk(serviceTrunks, trunk->id);

    if (serviceTrunks.size() == 0) {
        if (trace != nullptr) {
            *trace << "DEBUG|SERVICE TRUNK DECLINE|CAUSE SERVICE TRUNK NOT FOUND BY TRUNK " << trunk->name <<
                   " (" << trunk->id << ")" << "\n";
        }
        return;
    }

    for (auto serviceTrunk : serviceTrunks) {
        vector<ServiceTrunkSettings *> trunkSettingsList;
        getAllServiceTrunkSettings(trunkSettingsList, serviceTrunk->id, destinationType);

        for (auto trunkSettings : trunkSettingsList) {

            if (checkNNPTrunkSettingsConditions(trunkSettings, srcNumber, dstNumber)) {

                auto account = getAccount(serviceTrunk->client_account_id);
                if (account == nullptr) {
                    if (trace != nullptr) {
                        *trace << "DEBUG|TRUNK SETTINGS SKIP|ACCOUNT NOT FOUND BY ID " <<
                               serviceTrunk->client_account_id << "\n";
                    }
                    continue;
                }

                set<pair<double, int>> resultNNPPackagePriceIds;
                set<pair<double, int>> resultNNPPackagePricelistIds;

                findNNPPackagePriceIds(resultNNPPackagePriceIds, trunkSettings->nnp_tariff_id,
                                       nnpDestinationIds, trace);

                findNNPPackagePricelistIds(resultNNPPackagePricelistIds, trunkSettings->nnp_tariff_id,
                                           dstNumber, trace);

                if (resultNNPPackagePriceIds.size() == 0 && resultNNPPackagePricelistIds.size() == 0)
                    continue;

                set<pair<double, pair<int, int>>> resultNNPPackage;

                for (auto i:resultNNPPackagePriceIds)
                    resultNNPPackage.insert(make_pair(i.first, make_pair(0, i.second)));
                for (auto i:resultNNPPackagePricelistIds)
                    resultNNPPackage.insert(make_pair(i.first, make_pair(i.second, 0)));

                if (resultNNPPackage.size() == 0) {
                    if (trace != nullptr) {
                        *trace << "DEBUG|TRUNK SETTINGS DECLINE|NNP_PACKAGE NOT FOUND TRUNK_SETTINGS_ID: " <<
                               trunkSettings->id << " / " << trunkSettings->order << "\n";
                    }
                    continue;
                }

                pair<int, int> nnpPackage = (*(resultNNPPackage.begin())).second;

                ServiceTrunkOrder order;
                order.trunk = trunk;
                order.account = account;
                order.serviceTrunk = serviceTrunk;
                order.trunkSettings = trunkSettings;
                order.statsTrunkSettings = nullptr;
                order.pricelist = nullptr;

                if (nnpPackage.first > 0) {
                    order.nnpPackagePrice_id = nnpPackage.first;
                } else {
                    order.nnpPackagePricelist_id = nnpPackage.second;
                }

                order.nnpPackage_id = trunkSettings->nnp_tariff_id;

                if (trace != nullptr) {
                    *trace << "DEBUG|TRUNK SETTINGS ACCEPT|TRUNK_SETTINGS_ID: " << trunkSettings->id << " / " <<
                           trunkSettings->order << "\n";
                }
                order.nnp_price = (*(resultNNPPackage.begin())).first;
                order.price = nullptr;
                order.pricelist = nullptr;

                resultTrunkSettingsTrunkOrderList.push_back(order);
            }
        }

    }

    if (resultTrunkSettingsTrunkOrderList.size() > 0) {
        if (trace != nullptr) {
            *trace << "FOUND|TRUNK SETTING ORDER LIST|" << "\n";
        }

        for (auto order : resultTrunkSettingsTrunkOrderList) {
            if (trace != nullptr) {
                *trace << "||";
                order.dump(*trace);
                *trace << "\n";
            }
        }
    } else {
        if (trace != nullptr) {
            *trace << "NOT FOUND|TRUNK SETTING ORDER LIST|" << "\n";
        }
    }
}

bool Repository::checkNNPTrunkSettingsConditions(ServiceTrunkSettings *&trunkSettings, long long int srcNumber,
                                                 long long int dstNumber) {

    if (trunkSettings->src_number_id > 0 && !matchNumber(trunkSettings->src_number_id, srcNumber)) {
        if (trace != nullptr) {
            *trace << "DEBUG|TRUNK SETTINGS DECLINE|BY SRC NUMBER MATCHING, TRUNK_SETTINGS_ID: " <<
                   trunkSettings->id << " / " << trunkSettings->order << "\n";
        }
        return false;
    }

    if (trunkSettings->dst_number_id > 0 && !matchNumber(trunkSettings->dst_number_id, dstNumber)) {
        if (trace != nullptr) {
            *trace << "DEBUG|TRUNK SETTINGS DECLINE|BY DST NUMBER MATCHING, TRUNK_SETTINGS_ID: " <<
                   trunkSettings->id << " / " << trunkSettings->order << "\n";
        }
        return false;
    }

    return true;
}

bool Repository::getNNPDestinationByNumberRange(set<int> &nnpDestinationIds, NNPNumberRange *nnpNumberRange,
                                                stringstream *trace) {

    bool fResult = false;

    if (nnpNumberRange != nullptr) {
        vector<int> nnpPrefixIds;
        getNNPPrefixsByNumberRange(nnpPrefixIds, nnpNumberRange->id, trace);
        fResult = getNNPDestinationsByPrefix(nnpDestinationIds, nnpPrefixIds, trace);
    }
    if (trace != nullptr) {

        *trace << "INFO|NNP|NNP DESTINATION SET (";
        for (auto it = nnpDestinationIds.begin(); it != nnpDestinationIds.end(); it++) {
            NNPDestination *nnpDestination = getNNPDestination(*it);
            if (nnpDestination != nullptr) nnpDestination->dump(*trace);
            *trace << " ";
        }
        *trace << ")\n";
    }

    return fResult;
}

bool Repository::getNNPDestinationByNum(set<int> &nnpDestinationIds, long long int num, stringstream *trace) {
    bool fResult = false;

    NNPNumberRange *nnpNumberRange = getNNPNumberRange(num, trace);
    fResult = getNNPDestinationByNumberRange(nnpDestinationIds, nnpNumberRange, trace);
    if (trace != nullptr && !fResult) {
        *trace << "NOT FOUND|NNPDestination|BY num '" << num << "'" << "\n";

    }
    return fResult;
}

void Repository::findNNPPackagePricelistIds(set<pair<double, int>> &resultNNPPackagePricelistIds, int tariff_id,
                                            long long int num, stringstream *trace) {

    set<NNPPackagePricelist *> nnpPackagePricelistPtr;


    nnpPackagePricelist->findPackagePricelistIds(nnpPackagePricelistPtr, tariff_id, num, trace);

    for (auto item : nnpPackagePricelistPtr) {
        NNPPackagePricelist *package = &*item;
        PricelistPrice *pricelistPrice = getPrice(package->pricelist_id, num);
        if (pricelistPrice != nullptr) {
            double cost = pricelistPrice->price;

            resultNNPPackagePricelistIds.insert(pair<double, int>(cost, package->id));

            if (trace != nullptr) {
                *trace << "FOUND|NNP PACKAGE PRICELIST|BY NNP_TARIFF_ID '" << tariff_id << "'" << "\n";
                *trace << "||";
                package->dump(*trace);
                *trace << "\n";
            }
        }

    }

}

PhoneNumber
Repository::getNNPBestGeoRoute(PhoneNumber NumAdef, vector<PhoneNumber> &vNumA, PhoneNumber NumB, stringstream *trace) {

    NNPNumberRange *nr_numAdef = getNNPNumberRange(NumAdef);
    NNPNumberRange *nr_numB = getNNPNumberRange(NumB);

    vector<pair<PhoneNumber, NNPNumberRange>> vNr, vNrRes;

    if (nr_numAdef == nullptr) return -1;
    if (nr_numB == nullptr) return -1;

    if (trace != nullptr) {
        *trace << "DEBUG|FOUND NNP_NUMBER_RANGE|FOR NumbB: " << NumB;
        *trace << "||";
        nr_numB->dump(*trace);
        *trace << "\n";
    }

    vNr.push_back(make_pair(NumAdef, *nr_numAdef));

    for (auto inr : vNumA) {
        NNPNumberRange *nr_temp = getNNPNumberRange(inr);
        if (nr_temp != nullptr) vNr.push_back(make_pair(inr, *nr_temp));
    }

    for (auto inr: vNr) {
        if (inr.second.nnp_city_id == nr_numB->nnp_city_id) vNrRes.push_back(inr);
    }

    for (auto inr: vNr) {
        if (inr.second.nnp_region_id == nr_numB->nnp_region_id) vNrRes.push_back(inr);
    }

    if (vNrRes.size() > 0) {
        if (trace != nullptr) {
            *trace << "DEBUG|FOUND NNP_NUMBER_RANGE|BestGeoRoute from NumbA: " << vNrRes.begin()->first;
            *trace << "||";
            vNrRes.begin()->second.dump(*trace);
            *trace << "\n";
        }
        return vNrRes.begin()->first;
    }

    return -1;
}

pair<int, RadiusAuthRequest> Repository::getNNPRegionTrunkByNum(PhoneNumber numA, PhoneNumber numB) {

    RadiusAuthRequest result;
    int server_id = 0;
    ServiceNumber *serviceNumber = getServiceNumber(numA);

    if (serviceNumber != nullptr) {
        server_id = serviceNumber->server_id;
        result.srcNumber = to_string(numA);
        result.srcNoa = 3;
        result.dstNumber = to_string(numB);
        result.dstNoa = 3;
        Trunk *trunk = findAnyOurTrunk(server_id);
        if (trunk != nullptr) result.trunkName = trunk->name;
    }

    return make_pair(server_id, result);

}

void Repository::getNNPBestPriceRoute(set<pair<double, PhoneNumber>> &vResNum, vector<PhoneNumber> &vNumA,
                                      PhoneNumber NumB, stringstream *trace) {

    for (auto it : vNumA) {

        pair<int, RadiusAuthRequest> req = getNNPRegionTrunkByNum(it, NumB);

        if (req.first != 0 && req.second.trunkName.size() > 0
            && req.second.srcNumber.size() > 0 && req.second.dstNumber.size() > 0) {

            RadiusAuthRequest request = req.second;

            Cdr cdr;
            cdr.id = 0;
            cdr.connect_time = time(nullptr);
            cdr.session_time = 60;
            strncpy(cdr.src_number, request.srcNumber.c_str(), sizeof(cdr.src_number) - 1);
            strncpy(cdr.dst_number, request.dstNumber.c_str(), sizeof(cdr.dst_number) - 1);
            strncpy(cdr.redirect_number, request.redirectNumber.c_str(), sizeof(cdr.redirect_number) - 1);
            strncpy(cdr.src_route, request.trunkName.c_str(), sizeof(cdr.src_route) - 1);
            strncpy(cdr.dst_route, string("").c_str(), sizeof(cdr.dst_route) - 1);
            cdr.src_noa = request.srcNoa;
            cdr.dst_noa = request.dstNoa;
            cdr.call_id = 0;

            BillingCall billingCall(this);

            Call call = Call(&cdr, CALL_ORIG);
            CallInfo callInfo;
            billingCall.calc(&call, &callInfo, &cdr);

            vResNum.insert(make_pair(-call.cost, it));

        }


    }

}

bool Repository::NNPtrunkOrderLessThan(const ServiceTrunkOrder &left, const ServiceTrunkOrder &right) const {

    if (left.nnp_price < right.nnp_price) {
        return true;
    }
    return false;
}