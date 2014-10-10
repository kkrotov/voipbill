#include "ThreadCheckStartTable.h"
#include "../classes/AppBill.h"
#include "../classes/UdpControlClient.h"

ThreadCheckStartTable::ThreadCheckStartTable() {
    id = idName();
    name = "Check Start Table";
    db_rad.setCS(app().conf.db_rad);

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

                forceFinishCall(it->first, it->second);

            }

        }

        waitingForFinishCalls.clear();
    }
}

void ThreadCheckStartTable::readRadacctCalls() {

    radacctCalls.clear();
    BDbResult res = db_rad.query("select acctsessionid, acctuniqueid from radacct_voip_start");
    while (res.next()) {
        radacctCalls[res.get_s(0)] = res.get_s(1);
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

void ThreadCheckStartTable::forceFinishCall(string acctSessionId, string acctUniqueId) {

    bool deleted = false;
    BDbResult res =
            db_rad.query(
            "select force_finish_call('" + acctSessionId + "','" + lexical_cast<string>(time(NULL) - last_t) + "')");
    if (res.next())
        deleted = res.get_b(0);

    if (deleted) {
        Log::error("Force finish call " + acctUniqueId);
        countForceFinishedCalls++;
    }

}

void ThreadCheckStartTable::htmlfull(stringstream & html) {
    this->html(html);

    html << "Time loop: <b>" << t.sloop() << "</b><br/>\n";
    html << "Time full loop: <b>" << t.sfull() << "</b><br/>\n";
    html << "loops: <b>" << t.count << "</b><br/>\n";
    html << "<br/>\n";
    html << "countWaitingForFinishCalls: <b>" << countWaitingForFinishCalls << "</b><br/>\n";
    html << "countForceFinishedCalls: <b>" << countForceFinishedCalls << "</b><br/>\n";
}

