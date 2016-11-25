#pragma once

#include "BasePage.h"
#include "../models/Cdr.h"
#include "../models/Call.h"
#include "../classes/Billing.h"
#include "../classes/BillingCall.h"
#include "../classes/nnp/NNPResolver.h"
#include <string>
#include <regex>

using namespace std;

class PageTestNNP : public BasePage {
    Repository repository;

public:
    bool canHandle(std::string &path) {
        return path == "/test/nnpcalc";
    }

    void render(std::stringstream &html, map<string, string> &parameters) {

        string cmd, sNum;
        bool is_need_debug_trace = false;
        bool use_simple_format = false;

        try {

            if (!repository.prepare()) {
                html << "ERROR|BILLING NOT READY";
                return;
            }

            if (parameters.find("cmd") != parameters.end())
                cmd = parameters["cmd"];

            if (parameters.find("is_need_debug_trace") != parameters.end())
                is_need_debug_trace = true;

            if (parameters.find("use_simple_format") != parameters.end())
                use_simple_format = true;

            if (cmd == "getDestinationByNum") {

                if (parameters.find("num") != parameters.end()) {
                    sNum = parameters["num"];

                    long long int num = std::atoll(sNum.c_str());

                    if (num > 0) {
                        set<int> nnpDestinationIds;
                        repository.getNNPDestinationByNum(nnpDestinationIds, num,
                                                          is_need_debug_trace ? &html : nullptr);

                        Json::Value value;

                        for (auto iNNNPDestination : nnpDestinationIds) {

                            NNPDestination *nnpDestination = repository.getNNPDestination(iNNNPDestination);
                            if (nnpDestination != nullptr)
                                value["dest" + to_string(iNNNPDestination)] = nnpDestination->writeJsonValue();
                        }

                        html << value;
                    }
                }
            } else if (cmd == "getNumberRangeByNum") {

                if (parameters.find("num") != parameters.end()) {
                    sNum = parameters["num"];

                    long long int num = std::atoll(sNum.c_str());

                    if (num > 0) {
                        set<int> nnpDestinationIds;
                        NNPNumberRange *nnpNumberRange = repository.getNNPNumberRange(num, is_need_debug_trace ? &html
                                                                                                               : nullptr);
                        if (nnpNumberRange != nullptr)
                            html << nnpNumberRange->writeJsonValue();
                    }
                }
            } else if (cmd == "getBestGeoRoute") {
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

            } else if (cmd == "getBestPriceRoute") {
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

            } else {
                throw CalcException(
                        "unknow command:" + cmd);
            }

        }
        catch (CalcException &e) {
            html << "ERROR|NNP RESOLVER STOPPED|CAUSE " << e.message << "\n";
        }


    }


};



