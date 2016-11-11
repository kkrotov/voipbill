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
        try {

            if (!repository.prepare()) {
                html << "ERROR|BILLING NOT READY";
                return;
            }

            if (parameters.find("cmd") != parameters.end())
                cmd = parameters["cmd"];


            if (cmd == "getDestinationByNum") {

                if (parameters.find("num") != parameters.end()) {
                    sNum = parameters["num"];

                    long long int num = std::atoll(sNum.c_str());

                    if (num > 0) {
                        set<int> nnpDestinationIds;
                        repository.getNNPDestinationByNum(nnpDestinationIds, num, &html);
                    }
                }
            } else if (cmd == "getNumberRangeByNum") {

                if (parameters.find("num") != parameters.end()) {
                    sNum = parameters["num"];

                    long long int num = std::atoll(sNum.c_str());

                    if (num > 0) {
                        set<int> nnpDestinationIds;
                        NNPNumberRange *nnpNumberRange = repository.getNNPNumberRange(num, &html);
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
                    PhoneNumber number = repository.getNNPBestGeoRoute(NumAdef, vNumA, NumB, &html);
                    html << "BestGeoRoute:" << number << "\n";
                } else
                    throw CalcException(
                            "getBestGeoRoute: invalid arguments: num_a_def , num_b , num_a(.+) - must be phonenumbers");

            } else if (cmd == "getBestPriceRoute") {
                PhoneNumber NumB = 0;
                vector<PhoneNumber> vNumA;
                vector<pair<double, PhoneNumber>> vResNum;

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
                    repository.getNNPBestPriceRoute(vResNum, vNumA, NumB, &html);
                    for (auto it : vResNum) {
                        html << "BestPriceRoute: " << it.second << " price:" << it.first << "\n";
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



