#include "ThreadCheckStartTable.h"
#include "../classes/AppBill.h"
#include "../classes/UdpControlClient.h"

ThreadCheckStartTable::ThreadCheckStartTable() {
    id = idName();
    name = "Check Start Table";
    db_calls.setCS(app().conf.db_calls);

    last_t = 0;
    countWaitingForFinishCalls = 0;
    countForceFinishedCalls = 0;

    threadSleepSeconds = app().conf.udp_openca_select_interval;
}

void ThreadCheckStartTable::run() {
    if (UdpControlClient::ready() == false) return;

    readRadacctCalls();

    if (radacctCalls.size() == 0) {
        last_t = time(NULL);
        return;
    }

    readOpencaCalls();


    for (auto it = radacctCalls.begin(); it != radacctCalls.end(); ++it) {

        // if not found
        if (opencaCalls.find(it->first) == opencaCalls.end()) {

            waitingForFinishCalls[it->first] = it->second;
            countWaitingForFinishCalls++;

        }

    }

    last_t = time(NULL);

    if (waitingForFinishCalls.size() > 0) {

        ssleep(app().conf.udp_force_finish_call_interval);

        readRadacctCalls();

        for (auto it = waitingForFinishCalls.begin(); it != waitingForFinishCalls.end(); ++it) {

            // if found
            if (radacctCalls.find(it->first) != radacctCalls.end()) {

                forceFinishCall(it->first);

            }

        }

        waitingForFinishCalls.clear();
    }
}

void ThreadCheckStartTable::readRadacctCalls() {

    radacctCalls.clear();
    BDbResult res = db_calls.query("select call_id from calls_cdr.start");
    while (res.next()) {
        radacctCalls[res.get_s(0)] = true;
    }


}

void ThreadCheckStartTable::readOpencaCalls() {

    vector<string> openca_calls;
    if (UdpControlClient::select_calls(openca_calls) == false) {
        throw Exception("Can't receive current calls list", "ThreadCheckStartTable::readOpencaCalls");
    }

    opencaCalls.clear();
    for (auto it = openca_calls.begin(); it != openca_calls.end(); ++it) {
        opencaCalls[*it] = true;
    }
}

void ThreadCheckStartTable::forceFinishCall(string callId) {

    db_calls.exec("delete from calls_cdr.start where call_id = '" + callId + "'");

    Log::error("Force finish call " + callId);
    countForceFinishedCalls++;

}

void ThreadCheckStartTable::htmlfull(stringstream & html) {
    this->html(html);

    html << "countWaitingForFinishCalls: <b>" << countWaitingForFinishCalls << "</b><br/>\n";
    html << "countForceFinishedCalls: <b>" << countForceFinishedCalls << "</b><br/>\n";
}

bool ThreadCheckStartTable::hasFullHtml() {
    return true;
}
