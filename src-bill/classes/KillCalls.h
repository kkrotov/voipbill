#pragma once

#include "../lists/CurrentCallsObjList.h"
#include "UdpControlClient.h"
#include "../../src/common.h"
#include "../../src/classes/Log.h"

class KillCalls {
public:

    static void kill(CurrentCallsObjList *calls) {
        if (UdpControlClient::ready() == false) return;

        string phones;
        string ids;
        for (int i = 0; i < calls->count; i++) {
            pCallObj call = calls->get(i);
            if (call->kill_call_reason == 0) continue;

            if (phones != "") phones += ",";
            if (ids != "") ids += ",";

            phones += call->usage_num;
            ids += call->id;
        }

        if (phones.empty()) return;

        if (UdpControlClient::kill(phones, ids)) {
            for (int i = 0; i < calls->count; i++) {
                pCallObj call = calls->get(i);
                if (call->kill_call_reason == 0) continue;

                string reason;
                if (call->kill_call_reason == KILL_REASON_UNKNOWN_NUMBER)
                    reason = "Unknown number";
                else if (call->kill_call_reason == KILL_REASON_UNKNOWN_CLIENT)
                    reason = "Unknown client";
                else if (call->kill_call_reason == KILL_REASON_VOIP_DISABLED)
                    reason = "Void disabled";
                else if (call->kill_call_reason == KILL_REASON_CREDIT_LIMIT)
                    reason = "Credit limit";
                else if (call->kill_call_reason == KILL_REASON_DAYLY_LIMIT)
                    reason = "Daily limit";
                else if (call->kill_call_reason == KILL_REASON_MONTHLY_LIMIT)
                    reason = "Monthly limit";
                else
                    reason = "Reason" + lexical_cast<string>(call->kill_call_reason);


                Log::notice("KILL " + string(call->id) + " / " + string(call->usage_num) + " / " + reason);
            }
        }

    }

    static bool set_disable_status(map<string, bool> &phone_nums) {
        if (UdpControlClient::ready() == false) return false;

        string lock_phones;
        string unlock_phones;
        for (map < string, bool>::iterator i = phone_nums.begin(); i != phone_nums.end(); ++i) {
            if (i->second) {
                if (lock_phones != "") lock_phones += ",";
                lock_phones += i->first;
            } else {
                if (unlock_phones != "") unlock_phones += ",";
                unlock_phones += i->first;
            }
        }

        if (lock_phones.size() > 0) {
            if (UdpControlClient::lock(lock_phones) == false) return false;
        }

        if (unlock_phones.size() > 0) {
            if (UdpControlClient::unlock(unlock_phones) == false) return false;
        }
        return true;
    }

};
