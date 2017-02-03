#pragma once

#include "BasePage.h"
#include "../models/Cdr.h"
#include "../models/Call.h"
#include "../classes/Billing.h"
#include "../classes/BillingCall.h"
#include <string>

using namespace std;

class PageTestCalc : public BasePage {
public:
    bool canHandle(std::string &path) {
        return path == "/test/calc";
    }
    void render(std::stringstream &html, map<string, string> &parameters) {

        string orig;
        string connect_time;
        string session_time;
        string src_number;
        string dst_number;
        string redirect_number;
        string src_route;
        string dst_route;
        string src_noa;
        string dst_noa;

        if (parameters.find("orig") != parameters.end())
            orig = parameters["orig"];

        if (parameters.find("connect_time") != parameters.end())
            connect_time = parameters["connect_time"];

        if (parameters.find("session_time") != parameters.end())
            session_time = parameters["session_time"];

        if (parameters.find("src_number") != parameters.end())
            src_number = parameters["src_number"];

        if (parameters.find("dst_number") != parameters.end())
            dst_number = parameters["dst_number"];

        if (parameters.find("redirect_number") != parameters.end())
            redirect_number = parameters["redirect_number"];

        if (parameters.find("src_route") != parameters.end())
            src_route = parameters["src_route"];

        if (parameters.find("dst_route") != parameters.end())
            dst_route = parameters["dst_route"];

        if (parameters.find("src_noa") != parameters.end())
            src_noa = parameters["src_noa"];

        if (parameters.find("dst_noa") != parameters.end())
            dst_noa = parameters["dst_noa"];

        Cdr cdr;
        cdr.id = 0;
        cdr.connect_time = parseDateTime(connect_time.c_str());
        cdr.session_time = atoi(session_time.c_str());
        strncpy(cdr.src_number, src_number.c_str(), sizeof(cdr.src_number) - 1);
        strncpy(cdr.dst_number, dst_number.c_str(), sizeof(cdr.dst_number) - 1);
        strncpy(cdr.redirect_number, redirect_number.c_str(), sizeof(cdr.redirect_number) - 1);
        strncpy(cdr.src_route, src_route.c_str(), sizeof(cdr.src_route) - 1);
        strncpy(cdr.dst_route, dst_route.c_str(), sizeof(cdr.dst_route) - 1);
        cdr.src_noa = atoi(src_noa.c_str()) ? atoi(src_noa.c_str()) : 3;
        cdr.dst_noa = atoi(dst_noa.c_str()) ? atoi(dst_noa.c_str()) : 3;
        cdr.call_id = 0;

        html << "INFO|CDR|";
        cdr.dump(html);
        html << "\n";


        Repository repository;
        if (!repository.prepare(cdr.connect_time)) {
            html << "ERROR|BILLING NOT READY";
            return;
        }

        Billing billing;
        BillingCall billingCall(&repository);
        billingCall.setTrace(&html);

        StateMegaTrunk stateMegaTrunk(&repository);

        stateMegaTrunk.setTrace(&html);
        stateMegaTrunk.prepareFromCdr(&cdr); // Загружаем исходные данные для расчета МегаТранков из cdr- звонка.
        stateMegaTrunk.PhaseCalc(); // Расчет фаз маршутизации для Мегатранков

        Call call = Call(&cdr, orig == "true" ? CALL_ORIG : CALL_TERM);
        CallInfo callInfo;
        billingCall.calc(&call, &callInfo, &cdr, &stateMegaTrunk);

        html << "INFO|CALL|";
        call.dump(html);
        html << "\n";

        html << "RESULT|OK|rate = " << call.rate << ", cost = " << call.cost << "\n";
    }
};