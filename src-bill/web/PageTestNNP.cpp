#include "PageTestNNP.h"
#include "../classes/nnp/NNPPrefixImport.h"

void PageTestNNP::render(std::stringstream &html, map<string, string> &parameters) {

    try {

        if (!repository.prepare()) {

            html << "ERROR|BILLING NOT READY";
            return;
        }
        string cmd;
        if (parameters.find("cmd") != parameters.end())
            cmd = parameters["cmd"];

        if (parameters.find("is_need_debug_trace") != parameters.end())
            is_need_debug_trace = true;

        if (parameters.find("use_simple_format") != parameters.end())
            use_simple_format = true;

        if (cmd == "getDestinationByNum") {

            getDestinationByNum (html, parameters);
        }
        else if (cmd == "getNumberRangeByNum") {

            getNumberRangeByNum (html, parameters);
        }
        else if (cmd == "getBestGeoRoute") {

            getBestGeoRoute (html, parameters);
        }
        else if (cmd == "getBestPriceRoute") {

            getBestPriceRoute (html, parameters);
        }
        else if (cmd == "getPrefixListByDestinationID") {

            getPrefixListByDestinationID (html, parameters);
        }
        else if (cmd == "fillNNPPrefixList") {

            fillNNPPrefixList (html, parameters);
        }
        else {

            throw CalcException("unknow command:" + cmd);
        }
    }
    catch (CalcException &e) {

        html << "ERROR|NNP RESOLVER STOPPED|CAUSE " << e.message << "\n";
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void PageTestNNP::getDestinationByNum (std::stringstream &html, map<string, string> &parameters) {

    if (parameters.find("num") != parameters.end()) {
        string sNum = parameters["num"];

        long long int num = std::atoll(sNum.c_str());

        if (num > 0) {
            set<int> nnpDestinationIds;
            repository.getNNPDestinationByNum(nnpDestinationIds, num, is_need_debug_trace ? &html : nullptr);

            Json::Value value;

            for (auto iNNNPDestination : nnpDestinationIds) {

                NNPDestination *nnpDestination = repository.getNNPDestination(iNNNPDestination);
                if (nnpDestination != nullptr)
                    value["dest" + to_string(iNNNPDestination)] = nnpDestination->writeJsonValue();
            }

            html << value;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void PageTestNNP::getNumberRangeByNum (std::stringstream &html, map<string, string> &parameters) {

    if (parameters.find("num") != parameters.end()) {
        string sNum = parameters["num"];

        long long int num = std::atoll(sNum.c_str());

        if (num > 0) {
            set<int> nnpDestinationIds;
            NNPNumberRange *nnpNumberRange = repository.getNNPNumberRange(num, is_need_debug_trace ? &html
                                                                                                   : nullptr);
            if (nnpNumberRange==nullptr) {

                pair<int,int> prefix_code = repository.getNNPCountryPrefix(num);
                NNPNumberRange nnpNumberRange={0};
                nnpNumberRange.country_prefix = prefix_code.first;
                nnpNumberRange.country_code = prefix_code.second;
                html << nnpNumberRange.writeJsonValue();
            }
            else {

                if (nnpNumberRange->country_prefix==0) {

                    pair<int,int> prefix_code = repository.getNNPCountryPrefix(num);
                    nnpNumberRange->country_prefix = prefix_code.first;
                    nnpNumberRange->country_code = prefix_code.second;
                }
                if (nnpNumberRange->country_code==0)
                    nnpNumberRange->country_code = repository.getNNPCountryCode(nnpNumberRange->country_prefix);

                html << nnpNumberRange->writeJsonValue();
            }
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void PageTestNNP::getBestGeoRoute (std::stringstream &html, map<string, string> &parameters) {

    PhoneNumber NumAdef = 0, NumB = 0;
    vector<PhoneNumber> vNumA;

    auto parNumAdef = parameters.find("num_a_def");
    if (parNumAdef != parameters.end())
        NumAdef = std::atoll(parNumAdef->second.c_str());

    auto parNumB = parameters.find("num_b");
    if (parNumB != parameters.end())
        NumB = std::atoll(parNumB->second.c_str());

    for (auto it : parameters) {
        if (it.first != "num_a_def" &&
            it.first.find("num_a", 0) == 0 && it.first.size() > string("num_a").size()) {
            vNumA.push_back(std::atoll(it.second.c_str()));
        }
    }

    if (NumAdef > 0 && NumB > 0 && vNumA.size() > 0) {
        PhoneNumber number = repository.getNNPBestGeoRoute(NumAdef, vNumA, NumB,
                                                           is_need_debug_trace ? &html : nullptr);
        Json::Value value;
        value["src_num"] = number;

        if (use_simple_format) {
            html << number;
        } else {
            html << value;
        }


    } else
        throw CalcException(
                "getBestGeoRoute: invalid arguments: num_a_def , num_b , num_a(.+) - must be phonenumbers");
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void PageTestNNP::getBestPriceRoute (std::stringstream &html, map<string, string> &parameters) {

    PhoneNumber NumB = 0;
    vector<PhoneNumber> vNumA;
    set<pair<double, PhoneNumber>> vResNum;

    auto parNumB = parameters.find("num_b");
    if (parNumB != parameters.end())
        NumB = std::atoll(parNumB->second.c_str());

    for (auto it : parameters) {
        if (it.first != "num_a_def" &&
            it.first.find("num_a", 0) == 0 && it.first.size() > string("num_a").size()) {
            vNumA.push_back(std::atoll(it.second.c_str()));
        }
    }

    if (NumB > 0 && vNumA.size() > 0) {
        repository.getNNPBestPriceRoute(vResNum, vNumA, NumB, is_need_debug_trace ? &html : nullptr);

        Json::Value value;

        int i = 1;
        string simple;

        for (auto it : vResNum) {
            Json::Value subvalue;
            subvalue["src_num"] = it.second;
            subvalue["price"] = it.first;
            value["num" + to_string(i)] = subvalue;
            simple += to_string(it.first) + "," + to_string(it.second) + "||";
            i++;
        }
        if (use_simple_format) {
            html << simple;
        } else {
            html << value;
        }

    } else
        throw CalcException(
                "getBestPriceRoute: invalid arguments: num_b , num_a(.+) - must be phonenumbers");
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void PageTestNNP::getPrefixListByDestinationID (std::stringstream &html, map<string, string> &parameters) {

    if (parameters.find("id") != parameters.end()) {

        string sNum = parameters["id"];

        int num = std::atoi(sNum.c_str());

        std::vector<PhoneNumber> list;
        repository.getPrefixByNNPDestination(list,num);

        Json::Value main;

        main["size"] = boost::lexical_cast<int>(list.size());
        Json::Value enumiration;
        for (int i = 0; i < list.size();i++) {
            enumiration[boost::lexical_cast<int>(i)] = boost::lexical_cast<PhoneNumber>(list.at(i));
        }
        main["list"] = enumiration;

        html << main;

    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int PageTestNNP::getIntParam(map<string, string> &parameters, string name) {

    if (parameters.find(name) == parameters.end())
        return 0;

    return std::atoi(parameters[name].c_str());
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void PageTestNNP::fillNNPPrefixList (std::stringstream &html, map<string, string> &parameters) {

    int prefixlist_id = getIntParam(parameters, "id");
    if (prefixlist_id==0) {
        return;
    }
    Json::Value testNnpStatus;
    testNnpStatus["cmd"] = "fillNNPPrefixList";

    NNPPrefixImport nnpPrefixList;
    if (!nnpPrefixList.read(prefixlist_id)) {

        testNnpStatus["message"] = nnpPrefixList.err_message();
        testNnpStatus["status"] = "FAILED";
        html << testNnpStatus;
        return;
    }
    if (!nnpPrefixList.is_valid()) {

        testNnpStatus["message"] = "Invalid nnp_filter_json field value ";
        testNnpStatus["status"] = "FAILED";
        html << testNnpStatus;
        return;
    }
    if (nnpPrefixList.type_id()!=6) {

        testNnpStatus["message"] = "Invalid type_id";
        testNnpStatus["status"] = "FAILED";
        html << testNnpStatus;
        return;
    }
    if (!nnpPrefixList.save(prefixlist_id)) {

        testNnpStatus["message"] = nnpPrefixList.err_message();
        testNnpStatus["status"] = "FAILED";
        html << testNnpStatus;
        return;
    }
    if (nnpPrefixList.destination_id()>0) {

        testNnpStatus["nnp_destination_id"] = nnpPrefixList.destination_id();
    }
    else {

        testNnpStatus["country_code"] = nnpPrefixList.country_code();
        testNnpStatus["operator_id"] = nnpPrefixList.operator_id();
        testNnpStatus["region_id"] = nnpPrefixList.region_id();
        testNnpStatus["city_id"] = nnpPrefixList.city_id();
        testNnpStatus["ndc_type_id"] = nnpPrefixList.ndc_type_id();
    }
    testNnpStatus["type_id"] = nnpPrefixList.type_id();
    testNnpStatus["prefix_list_size"] = nnpPrefixList.prefix_list_size();
    testNnpStatus["status"] = "SUCCESS";
    html << testNnpStatus;
}