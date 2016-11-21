#include "Repository.h"
#include "RadiusAuthProcessor.h"
#include "BillingCall.h"

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