#pragma once

#include "BasePage.h"
#include "../models/Cdr.h"
#include "../models/Call.h"
#include "../classes/Billing.h"
#include "../classes/BillingCall.h"
#include "../classes/nnp/NNPResolver.h"
#include <string>

using namespace std;

class PageTestNNP : public BasePage {
    Repository repository;

public:
    bool canHandle(std::string &path) {
        return path == "/test/nnpcalc";
    }

    void render(std::stringstream &html, map<string, string> &parameters) {

        string cmd, sNum;

        if (parameters.find("cmd") != parameters.end())
            cmd = parameters["cmd"];


        if (cmd == "getDestinationByNum") {

            if (parameters.find("num") != parameters.end()) {
                sNum = parameters["num"];

                long long int num = std::atoll(sNum.c_str());

                if (num > 0)

                    try {

                        if (!repository.prepare()) {
                            html << "ERROR|BILLING NOT READY";
                            return;
                        }

                        set<int> nnpDestinationIds;
                        repository.getNNPDestinationByNum(nnpDestinationIds, num, &html);

                    }
                    catch (CalcException &e) {
                        html << "ERROR|NNP RESOLVER STOPPED|CAUSE " << e.message << "\n";
                    }
            }
        }

    }


};



