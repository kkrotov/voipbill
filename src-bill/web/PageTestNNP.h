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
public:
    bool canHandle(std::string &path) {
        return path == "/test/nnpcalc";
    }

    void render(std::stringstream &html, map<string, string> &parameters) {

        string cmd;

        if (parameters.find("cmd") != parameters.end()) {
            cmd = parameters["cmd"];
            //html << cmd;
        } else

            html << "not found";

        try {

            Repository repository;
            if (!repository.prepare()) {
                html << "ERROR|BILLING NOT READY";
                return;
            }

            NNPResolver nnp = NNPResolver(&repository);
            nnp.setTrace(&html);

            bool res = nnp.matchNumberNNPDestination(79263747216, 3);

            html << "Номер 79263747216 - " << res << "\n";

        } catch (CalcException &e) {
            html << "ERROR|NNP RESOLVER STOPPED|CAUSE " << e.message << "\n";
        }

    }
};



