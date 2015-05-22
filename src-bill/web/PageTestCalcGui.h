#pragma once

#include "BasePage.h"
#include "../models/Cdr.h"
#include "../models/Call.h"
#include "../classes/Billing.h"
#include "../classes/BillingCall.h"
#include "../data/DataCurrentCallsContainer.h"
#include <string>

using namespace std;

class PageTestCalcGui : public BasePage {
public:
    bool canHandle(std::string &path) {
        return path == "/test/calc/gui";
    }
    void render(std::stringstream &html, map<string, string> &parameters) {

        renderHeader(html);

        long long int call_id = 0;
        bool orig = true;

        if (parameters.find("call_id") != parameters.end())
            call_id = atoll(parameters["call_id"].c_str());

        if (parameters.find("orig") != parameters.end())
            orig = parameters["orig"] != "f";

        auto currentCdr = DataCurrentCallsContainer::instance()->currentCdr.get();
        if (currentCdr == nullptr) {
            html << "BILLING NOT READY";
            return;
        }

        Cdr * cdr = nullptr;
        for (size_t i = 0; i < currentCdr->size(); i++) {
            if (currentCdr->get(i)->call_id == call_id) {
                cdr = currentCdr->get(i);
                break;
            }
        }
        if (cdr == nullptr) {
            html << "CDR WITH CALL ID #" << call_id << " NOT FOUND";
            return;
        }

        PreparedData preparedData;
        if (!DataContainer::instance()->prepareData(preparedData, cdr->connect_time)) {
            html << "BILLING NOT READY";
            return;
        }


        std::stringstream trace;

        trace << "INFO|CDR|";
        cdr->dump(trace);
        trace << "\n";


        Billing billing;
        BillingCall billingCall(&billing);

        Call call = Call(cdr, orig);

        if (!orig) {
            Call callOrig = Call(cdr, CALL_ORIG);
            billingCall.calc(&callOrig, cdr, &preparedData);
            call.src_number = callOrig.src_number;
            call.dst_number = callOrig.dst_number;
        }

        billingCall.setTrace(&trace);
        billingCall.calc(&call, cdr, &preparedData);

        trace << "INFO|CALL|";
        call.dump(trace);
        trace << "\n";

        trace << "RESULT|OK|rate = " << call.rate << ", cost = " << call.cost << "\n";

        string traceResult = trace.str();
        vector<string> traceLines;
        split(traceLines, traceResult, is_any_of("\n"));

        html << "<table border=0>\n";
        for(auto traceLine : traceLines) {
            vector<string> traceColumns;
            split(traceColumns, traceLine, boost::is_any_of("|"));
            string column1 = traceColumns.size() > 0 ? traceColumns.at(0) : "";
            string column2 = traceColumns.size() > 1 ? traceColumns.at(1) : "";
            string column3 = traceColumns.size() > 2 ? traceColumns.at(2) : "";

            if (column1 == "") continue;

            html << "<tr>\n";
            html << "<td nowrap style='text-align: left'>" << column1 << "</td>\n";
            html << "<td nowrap style='text-align: left'>" << column2 << "</td>\n";
            html << "<td style='text-align: left'>" << column3 << "</td>\n";
            html << "</tr>\n";
        }
        html << "</table>\n";
    }
};