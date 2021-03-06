#include "Repository.h"
#include "RadiusAuthProcessor.h"
#include "BillingCall.h"
#include "nnp/PrefixTree.h"
#include <stdio.h>


bool Repository::checkNNPTrunkSettingsConditions(ServiceTrunkSettings *&trunkSettings, long long int srcNumber,
                                                 long long int dstNumber) {

    if(trunkSettings->nnp_tariff_id == 0) {
        if (trace != nullptr) {
            *trace << "DEBUG|TRUNK SETTINGS DECLINE|BY NNP_TARIFF_ID IS 0 , TRUNK_SETTINGS_ID: " <<
                   trunkSettings->id << " / " << trunkSettings->order << "\n";
        }
        return false;
    }

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
        *trace << string("NOT FOUND|NNPDestination|BY num '") << num << "'" << "\n";

    }
    return fResult;
}

bool Repository::getPrefixByNNPDestination(vector<PhoneNumber> &prefixList, int destinationId) {

    std::vector<int> prefixPlus, prefixMinus, rangePlus, rangeMinus;
    this->nnpPrefixDestination->getPrefixsByNNPDestination(destinationId, prefixPlus, prefixMinus);

    for (int i : prefixPlus)
        this->nnpNumberRangePrefix->getNumberRangesByNNPPrefix(rangePlus, i);
    for (int i : prefixMinus)
        this->nnpNumberRangePrefix->getNumberRangesByNNPPrefix(rangeMinus, i);

    PrefixTree plusTree  (10000);
    PrefixTree minusTree (10000);

    for (int i = 0; i < rangePlus.size(); i++) {
        NNPNumberRange* range = this->nnpNumberRange->getNNPNumberRangeById(rangePlus.at(i));
        if ( !range )
            continue;
        std::pair<PhoneNumber, PhoneNumber> pair = {range->full_number_from, range->full_number_to};
        plusTree.addRange(pair);
    }
    for (int i = 0; i < rangeMinus.size(); i++) {
        NNPNumberRange* range = this->nnpNumberRange->getNNPNumberRangeById(rangeMinus.at(i));
        if ( !range )
            continue;
        std::pair<PhoneNumber, PhoneNumber> pair = {range->full_number_from, range->full_number_to};
        minusTree.addRange (pair);
    }
    plusTree.subTree(minusTree);

    char array[32] = {};
    plusTree.print(array, 0, 0, prefixList);

    return true;

}

void Repository::getPrefixByFilter (std::vector<PhoneNumber>& prefixes, int country_code, vector<int> operator_list, bool oper_excluded,
                                    int region_id, int city_id, int ndc_type_id) {
    PrefixTree plusTree  (100);
    if ( !this || !this->nnpNumberRange)
        return ;

    size_t size = this->nnpNumberRange->size();
    for (size_t i = 0; i < size; i++) {
        const NNPNumberRange* rangePtr = this->nnpNumberRange->get(i);

        if (!rangePtr)
            continue;

        if ( country_code && rangePtr->country_code != country_code )
            continue;

        if ( operator_list.size()) {

            if (std::find(operator_list.begin(), operator_list.end(), rangePtr->nnp_operator_id) != operator_list.end()) {
                // operator_list contains rangePtr->nnp_operator_id
                if (oper_excluded)
                    continue;
            } else {
                // operator_list does not contain rangePtr->nnp_operator_id
                if (!oper_excluded)
                    continue;
            }
        }
        if ( region_id && rangePtr->nnp_region_id != region_id )
            continue;
        if ( city_id && rangePtr->nnp_city_id != city_id )
            continue;
        if ( ndc_type_id && rangePtr->ndc_type_id != ndc_type_id )
            continue;

        std::pair<PhoneNumber, PhoneNumber> range = {rangePtr->full_number_from, rangePtr->full_number_to};
        plusTree.addRange(range);
    }
    char array[32] = {};
    plusTree.print(array, 0, 0, prefixes);

    return ;
}

void Repository::findNNPPackagePricelistIds(set<pair<double, NNPPackagePricelist *>> &resultNNPPackagePricelistIds,
                                            int tariff_id,
                                            long long int num, stringstream *trace) {

    set<NNPPackagePricelist *> nnpPackagePricelistPtr;


    nnpPackagePricelist->findPackagePricelistIds(nnpPackagePricelistPtr, tariff_id, num, trace);

    for (auto item : nnpPackagePricelistPtr) {
        NNPPackagePricelist *package = &*item;
        PricelistPrice *pricelistPrice = getPrice(package->pricelist_id, num);
        if (pricelistPrice != nullptr) {
            double cost = pricelistPrice->price;

            resultNNPPackagePricelistIds.insert(pair<double, NNPPackagePricelist *>(cost, package));

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
            StateMegaTrunk stateMegaTrunk(this);

            stateMegaTrunk.setTrace(trace);
            stateMegaTrunk.prepareFromCdr(&cdr); // Загружаем исходные данные для расчета МегаТранков из cdr- звонка.
            stateMegaTrunk.PhaseCalc(); // Расчет фаз маршутизации для Мегатранков

            Call call = Call(&cdr, CALL_ORIG);
            CallInfo callInfo;
            billingCall.calc(&call, &callInfo, &cdr, &stateMegaTrunk);

            vResNum.insert(make_pair(-call.cost, it));

        }


    }

}
