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

        long long int num = std::atoll(sNum.c_str());

        if (num > 0)

            try {

                if (!repository.prepare()) {
                    html << "ERROR|BILLING NOT READY";
                    return;
                }

                NNPResolver nnp = NNPResolver(&repository);
                nnp.setTrace(&html);

                html << "Тестируем вхождение в диапазон номера " << num << "<br>\n";

                NNPNumberRange *nnpNumberRange = repository.getNNPNumberRange(num, &html);

                if (nnpNumberRange != nullptr) {
                    html << "Нашли: " << nnpNumberRange->id << " [" << nnpNumberRange->full_number_to << ":" <<
                    nnpNumberRange->full_number_to << "]<br>\n";

                    html << "Получаем список префиксов<br>\n";

                    vector<NNPNumberRangePrefix *> nnpNumberRangePrefixList;
                    repository.getNNPPrefixsByNumberRange(nnpNumberRangePrefixList, nnpNumberRange->id, &html);


                } else "Не нашли.<br>\n";


            } catch (CalcException &e) {
                html << "ERROR|NNP RESOLVER STOPPED|CAUSE " << e.message << "\n";
            }

    }


};



