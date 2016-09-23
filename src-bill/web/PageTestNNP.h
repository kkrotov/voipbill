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

        if (parameters.find("num") != parameters.end())
            sNum = parameters["num"];
        //html << cmd;

        if (std::atoll(sNum.c_str()) > 0)

            try {

                if (!repository.prepare()) {
                    html << "ERROR|BILLING NOT READY";
                    return;
                }

                NNPResolver nnp = NNPResolver(&repository);
                nnp.setTrace(&html);

                test(std::atoll(sNum.c_str()), html);

            } catch (CalcException &e) {
                html << "ERROR|NNP RESOLVER STOPPED|CAUSE " << e.message << "\n";
            }

    }

    void test(long long int num, std::stringstream &html) {
        vector<int> nnpMatchNumberRangeIds;

        html << "Тестируем вхождение в диапазоны номера " << num << "<br>\n";

        NNPNumberRange *nnpNumberRange = repository.getNNPNumberRange(num, &html);

        if (nnpNumberRange != nullptr) {
            html << "Нашли: " << nnpNumberRange->id << " [" << nnpNumberRange->full_number_to << ":" <<
            nnpNumberRange->full_number_to << "]<br>\n";
        } else "Не нашли.<br>\n";


    }

};



