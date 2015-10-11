#include <vector>

#include "../common.h"
#include "Log.h"
#include "UdpMessageProcessorBill.h"

UdpMessageProcessorBill::UdpMessageProcessorBill(const string &message, BDb * db_calls) {
    this->message = message;
    this->db_calls = db_calls;
}

void UdpMessageProcessorBill::parseRequest() {

    vector<string> parameters;
    split(parameters, message, boost::algorithm::is_any_of(";"));

    for (auto it = parameters.begin(); it != parameters.end(); ++it) {
        vector<string> pair;
        split(pair, *it, boost::algorithm::is_any_of(":"));

        if (pair.size() == 2) {
            string name = pair.at(0);
            string value = pair.at(1);

            if (name == "calling") {
                aNumber = value;
            } else if (name == "called") {
                bNumber = value;
            } else if (name == "trunk") {
                trunkNumber = atoi(value.c_str());
            }
        }
    }
}

bool UdpMessageProcessorBill::validateRequest() {

    if (aNumber == "") {
        throw new Exception("Udp request validation: bad calling: " + message, "UdpMessageProcessorBill::validateRequest");
    }

    if (bNumber == "") {
        throw new Exception("Udp request validation: bad called: " + message, "UdpMessageProcessorBill::validateRequest");
    }

    if (trunkNumber < 80) {
        throw new Exception("Udp request validation: bad trunk: " + lexical_cast<string>(trunkNumber), "UdpMessageProcessorBill::validateRequest");
    }

}

string UdpMessageProcessorBill::process() {

    try {

        parseRequest();

        validateRequest();

        calculateCall();

        return analyzeCall();

    } catch (Exception &e) {
        e.addTrace("UdpMessageProcessorBill ");
        Log::exception(e);
    } catch (std::exception &e) {
        Log::error("UdpMessageProcessorBill: " + string(e.what()));
    } catch (...) {
        Log::error("UdpMessageProcessorBill: ERROR");
    }

    return string("accept");
}

string UdpMessageProcessorBill::analyzeCall() {
    /*
    if (call.kill_call_reason == 0) {
        return "accept";
    } else if (call.kill_call_reason == KILL_REASON_CREDIT_LIMIT) {
        return "low_balance";
    } else if (
            call.kill_call_reason == KILL_REASON_VOIP_DISABLED
            || call.kill_call_reason == KILL_REASON_DAILY_LIMIT
            || call.kill_call_reason == KILL_REASON_MONTHLY_LIMIT
            ) {
        return "voip_disabled";
    } else {
        return "reject";
    }
    */
    return "";
}

void UdpMessageProcessorBill::calculateCall() {
/*
    shared_ptr<CurrentCallsObjList> splist = ThreadSelectCurrentCalls::getList();
    CurrentCallsObjList * list = splist.get();

    CalcFull calculator;

    calculator.setDb(db_calls);

    calculator.calc_current(list);

    prepareCall();

    calculator.calc_call(&call);

    calculator.calc_process_call(&call);
    */
}

void UdpMessageProcessorBill::prepareCall() {
    /*
    strcpy((char*) &call.id, "0");
    call.id_num = 0;
    call.time[0] = 0;
    call.len = 60;
    call.out = true;
    strcpy((char*) &call.usage_num, aNumber.c_str());
    strcpy((char*) &call.phone_num, bNumber.c_str());
    call.instance_id = trunkNumber;
    call.operator_id = 0;
    call.kill_call_reason = 0;
    call.prefix_geo[0] = 0;
    call.prefix_mcn[0] = 0;
    call.prefix_op[0] = 0;
    */
}