#include "PageTestNNP.h"

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

    Prefixlist prefixlist;
    string message;
    if (!getMainPrefixList(prefixlist_id, prefixlist, message)) {

        testNnpStatus["message"] = message;
        testNnpStatus["status"] = "FAILED";
        html << testNnpStatus;
        return;
    }
    testNnpStatus["nnp_destination_id"] = prefixlist.nnp_destination_id;
    testNnpStatus["type_id"] = prefixlist.type_id;
    testNnpStatus["country_code"] = prefixlist.country_code;
    testNnpStatus["operator_id"] = prefixlist.operator_id;
    testNnpStatus["region_id"] = prefixlist.region_id;
    testNnpStatus["city_id"] = prefixlist.city_id;
    testNnpStatus["ndc_type_id"] = prefixlist.ndc_type_id;
    if (prefixlist.nnp_destination_id==0 && prefixlist.country_code==0 && prefixlist.operator_id==0 &&
        prefixlist.region_id==0 && prefixlist.city_id==0 && prefixlist.ndc_type_id==0) {

        testNnpStatus["message"] = "Invalid nnp_filter_json field value ";
        testNnpStatus["status"] = "FAILED";
        html << testNnpStatus;
        return;
    }
    if (prefixlist.type_id!=6) {

        testNnpStatus["message"] = "Invalid type_id";
        testNnpStatus["status"] = "FAILED";
        html << testNnpStatus;
        return;
    }
    std::vector<PhoneNumber> phoneList;
    if (prefixlist.nnp_destination_id>0) {

        NNPDestination *nnpDestination = repository.getNNPDestination (prefixlist.nnp_destination_id);
        if (nnpDestination== nullptr) {

            testNnpStatus["message"] = "Invalid nnp_destination_id";
            testNnpStatus["status"] = "FAILED";
            html << testNnpStatus;
            return;
        }
        repository.getPrefixByNNPDestination (phoneList, prefixlist.nnp_destination_id);
    }
    else {

        repository.getPrefixByFilter(phoneList, prefixlist.country_code, prefixlist.operator_id, prefixlist.region_id,
                                     prefixlist.city_id, prefixlist.ndc_type_id);
    }
    testNnpStatus["prefix_list_size"] = std::to_string(phoneList.size());
    if (savePrefixList(prefixlist_id, phoneList, message)) {

        testNnpStatus["status"] = "SUCCESS";
    }
    else {

        testNnpStatus["message"] = message;
        testNnpStatus["status"] = "FAILED";
    }
    html << testNnpStatus;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool PageTestNNP::getMainPrefixList (int prefixlist_id, Prefixlist &prefixlist, string &errormessage) {

    try {

        BDb db_main(app().conf.db_main);
        string sql = "select id, name, type_id, "
                             "nnp_filter_json ->> 'nnp_destination_id' as nnp_destination_id, "
                             "nnp_filter_json ->> 'country_code' as country_code, "
                             "nnp_filter_json ->> 'operator_id' as operator_id, "
                             "nnp_filter_json ->> 'region_id' as region_id, "
                             "nnp_filter_json ->> 'city_id' as city_id, "
                             "nnp_filter_json ->> 'ndc_type_id' as ndc_type_id "
                     "from auth.prefixlist "
                     "where id="+std::to_string(prefixlist_id);

        BDbResult row = db_main.query(sql);
        if (!row.next()) {

            errormessage = "Invalid prefixlist_id";
            return false;
        }
        prefixlist.id = row.get_i(0);
        row.fill_cs(1, prefixlist.name, sizeof(prefixlist.name));
        prefixlist.type_id = row.get_i(2);
        prefixlist.nnp_destination_id = row.get_i(3);
        prefixlist.country_code = row.get_i(4);
        prefixlist.operator_id = row.get_i(5);
        prefixlist.region_id = row.get_i(6);
        prefixlist.city_id = row.get_i(7);
        prefixlist.ndc_type_id = row.get_i(8);
        return true;
    }
    catch (DbException &e) {

        errormessage = e.what();
        Log::error(e.what());
        return false;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool PageTestNNP::savePrefixList (int prefixlist_id, std::vector<PhoneNumber> phoneList, string &errormessage) {

    string sql = "DELETE FROM auth.prefixlist_prefix WHERE prefixlist_id="+std::to_string(prefixlist_id)+"; ";
    if (phoneList.size()>0) {

        sql += "INSERT INTO auth.prefixlist_prefix(prefixlist_id, prefix) VALUES ";
        string phonePrefixList;
        for (auto it = phoneList.begin(); it != phoneList.end(); ++it) {

            if (phonePrefixList.size() > 0)
                phonePrefixList += ",";

            string recPrefix = "(" + std::to_string(prefixlist_id) + ",'" + std::to_string(*it) + "')";
            phonePrefixList += recPrefix;
        }
        sql += phonePrefixList;
    }
    try {

        BDb db_main(app().conf.db_main);
        db_main.exec(sql);
        return true;
    }
    catch (DbException &e) {

        errormessage = e.what();
        Log::error(e.what());
        return false;
    }
}