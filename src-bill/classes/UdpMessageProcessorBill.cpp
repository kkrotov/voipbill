#include <vector>

#include "../common.h"
#include "Log.h"
#include "UdpMessageProcessorBill.h"

UdpMessageProcessorBill::UdpMessageProcessorBill(const string &aNumber, const string &bNumber, const string &trunkName) {
    this->aNumber = aNumber;
    this->bNumber = bNumber;
    this->trunkName = trunkName;
}

string UdpMessageProcessorBill::process() {

    try {

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