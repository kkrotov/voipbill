#pragma once

#include "BasePage.h"
#include "../models/Cdr.h"
#include "../models/Call.h"
#include "../classes/Billing.h"
#include "../classes/BillingCall.h"
#include <string>
#include "../classes/RadiusAuthProcessor.h"
#include "../classes/RadiusAuthServer.h"

using namespace std;

class PageTestAuth: public BasePage {
public:
    bool canHandle(std::string &path) {
        return path == "/test/auth";
    }
    void render(std::stringstream &html, map<string, string> &parameters) {

        RadiusAuthRequest request;
        RadiusAuthResponse response;
        pLogMessage logRequest(new LogMessage);

        if (parameters.find("trunk_name") != parameters.end())
            request.trunkName = parameters["trunk_name"];

        if (parameters.find("src_number") != parameters.end())
            request.srcNumber = parameters["src_number"];

        if (parameters.find("dst_number") != parameters.end())
            request.dstNumber = parameters["dst_number"];

        if (parameters.find("redirect_number") != parameters.end())
            request.redirectNumber = parameters["redirect_number"];

        if (parameters.find("src_noa") != parameters.end())
            request.srcNoa = atoi(parameters["src_noa"].c_str());

        if (parameters.find("dst_noa") != parameters.end())
            request.dstNoa = atoi(parameters["dst_noa"].c_str());

        html << "INFO|REQUEST|";
        request.dump(html);
        html << "\n";


        Repository repository;
        if (!repository.prepare()) {
            html << "ERROR|BILLING NOT READY";
            return;
        }

        RadiusAuthProcessor processor(&request, &response, logRequest);
        processor.setTrace(&html);
        processor.process();

        if (!response.accept) {
            html << "RESULT|REJECT|";
        } else if (response.airp.size() > 0) {
            html << "RESULT|AIRP|" << response.airp;
        } else if (response.routeCase.size() > 0) {
            html << "RESULT|ROUTE CASE|" << response.routeCase;
        } else if (response.releaseReason.size() > 0) {
            html << "RESULT|RELEASE REASON|" << response.releaseReason;
        } else {
            html << "RESULT|UNKNOWN|";
        }
        if (response.srcNumber.size() > 0) {
            html << ", calling = " << response.srcNumber;
        }
        if (response.dstNumber.size() > 0) {
            html << ", called = " << response.dstNumber;
        }
        html << "\n";
    }
};
