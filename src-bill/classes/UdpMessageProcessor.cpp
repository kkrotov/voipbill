#include <vector>

#include "../../src/common.h"
#include "../../src/classes/Log.h"
#include "UdpMessageProcessor.h"
#include "AppBill.h"
#include "../classes/DataLoader.h"
#include "CalcFull.h"
#include "../threads/ThreadSelectCurrentCalls.h"

UdpMessageProcessor::UdpMessageProcessor(const string &message, BDb * db_calls) {
    this->message = message;
    this->db_calls = db_calls;
}

void UdpMessageProcessor::parseRequest() {

    vector<string> parameters;
    split(parameters, message, boost::algorithm::is_any_of(";"));

    for (auto it = parameters.begin(); it != parameters.end(); ++it) {
        vector<string> pair;
        split(pair, *it, boost::algorithm::is_any_of(":"));

        if (pair.size() == 2) {
            string name = pair.at(0);
            string value = pair.at(1);

            if (name == "calling") {
                callingStationId = value;
            } else if (name == "called") {
                calledStationId = value;
            }
        }
    }
}

bool UdpMessageProcessor::validateRequest() {

    if (callingStationId == "") {
        throw new Exception("Udp request validation: bad calling: " + message, "UdpMessageProcessor::validateRequest");
    }

    if (calledStationId == "") {
        throw new Exception("Udp request validation: bad called: " + message, "UdpMessageProcessor::validateRequest");
    }

}

string UdpMessageProcessor::process() {

    try {

        parseRequest();

        validateRequest();

        calculateCall();

        return analyzeCall();

    } catch (Exception &e) {
        e.addTrace("UdpMessageProcessor ");
        Log::exception(e);
    } catch (std::exception &e) {
        Log::error("UdpMessageProcessor: " + string(e.what()));
    } catch (...) {
        Log::error("UdpMessageProcessor: ERROR");
    }

    return string("accept");
}

string UdpMessageProcessor::analyzeCall() {
    if (call.kill_call_reason == 0) {
        return "accept";
    } else if (call.kill_call_reason == KILL_REASON_CREDIT_LIMIT) {
        return "low_balance";
    } else if (
            call.kill_call_reason == KILL_REASON_VOIP_DISABLED
            || call.kill_call_reason == KILL_REASON_DAYLY_LIMIT
            || call.kill_call_reason == KILL_REASON_MONTHLY_LIMIT
            ) {
        return "voip_disabled";
    } else {
        return "reject";
    }
}

void UdpMessageProcessor::calculateCall() {

    shared_ptr<CurrentCallsObjList> splist = ThreadSelectCurrentCalls::getList();
    CurrentCallsObjList * list = splist.get();

    CalcFull calculator;

    calculator.setDb(db_calls);

    calculator.calc_current(list);

    prepareCall();

    calculator.calc_call(&call);

    calculator.calc_process_call(&call);
}

void UdpMessageProcessor::prepareCall() {
    time_t rawtime;
    struct tm * timeinfo;
    time(&rawtime);
    timeinfo = localtime(&rawtime);

    strcpy((char*) &call.id, "0");
    call.id_num = 0;
    call.time[0] = 0;
    call.dt.time = rawtime;
    call.dt.day = call.dt.time - timeinfo->tm_hour * 3600 - timeinfo->tm_min * 60 - timeinfo->tm_sec;
    call.dt.month = call.dt.day - (timeinfo->tm_mday - 1) * 86400;
    call.len = 60;
    call.out = true;
    strcpy((char*) &call.usage_num, callingStationId.c_str());
    strcpy((char*) &call.phone_num, calledStationId.c_str());
    call.instance_id = app().conf.instance_id;
    call.operator_id = 0;
    call.kill_call_reason = 0;
    call.prefix_geo[0] = 0;
    call.prefix_mcn[0] = 0;
    call.prefix_op[0] = 0;
}