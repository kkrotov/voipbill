#include "ThreadCheckStartTable.h"
#include "../classes/AppBill.h"
#include "../classes/UdpControlClient.h"

ThreadCheckStartTable::ThreadCheckStartTable() {
    id = idName();
    name = "Check Start Table";
    db_calls.setCS(app().conf.db_calls);

    countWaitingForFinishCalls = 0;
    countForceFinishedCalls = 0;

    threadSleepSeconds = app().conf.udp_openca_select_interval;
}

void ThreadCheckStartTable::run() {
    return;
    if (!UdpControlClient::ready()) return;
    if (!repository.currentCalls->ready()) return;
    //return;

    auto cdrs = repository.currentCalls->currentCdr.get();

    set<long long int> opencaCalls;
    fetchOpencaCurrentCalls(opencaCalls);

    set<long long int> waitingForFinishCalls;
    spawnWaitingForFinishCalls(waitingForFinishCalls, opencaCalls, cdrs.get());

    if (waitingForFinishCalls.size() > 0) {
        ssleep(app().conf.udp_force_finish_call_interval);
        forceFinishCalls(waitingForFinishCalls);
    }
}

void ThreadCheckStartTable::fetchOpencaCurrentCalls(set<long long int> &opencaCalls) {

    vector<string> openca_calls;
    if (!UdpControlClient::select_calls(openca_calls)) {
        throw Exception("Can't receive current calls list", "ThreadCheckStartTable::readOpencaCalls");
    }

    for (string &call_id : openca_calls) {
        opencaCalls.insert(atoll(call_id.c_str()));
    }
}

void ThreadCheckStartTable::spawnWaitingForFinishCalls(set<long long int> &waitingForFinishCalls, set<long long int> &opencaCalls, CurrentCdrList * cdrList) {

    for (size_t i = 0; i < cdrList->size(); ++i) {
        Cdr *cdr = cdrList->get(i);
        // if not found
        if (opencaCalls.find(cdr->call_id) == opencaCalls.end()) {
            waitingForFinishCalls.insert(cdr->call_id);
        }
    }

    countWaitingForFinishCalls = waitingForFinishCalls.size();
}

void ThreadCheckStartTable::forceFinishCalls(set<long long int> &waitingForFinishCalls) {

    auto newCdrs = repository.currentCalls->currentCdr.get();

    set<long long int> callsIds;
    for (size_t i = 0; i < newCdrs->size(); ++i) {
        Cdr *cdr = newCdrs->get(i);
        callsIds.insert(cdr->call_id);
    }

    for (long long int callId : waitingForFinishCalls) {
        // if found
        if (callsIds.find(callId) != callsIds.end()) {
            db_calls.exec("delete from calls_cdr.start where call_id = '" + lexical_cast<string>(callId) + "'");
            countForceFinishedCalls++;
        }
    }
}

void ThreadCheckStartTable::htmlfull(stringstream & html) {
    this->html(html);

    html << "countWaitingForFinishCalls: <b>" << countWaitingForFinishCalls << "</b><br/>\n";
    html << "countForceFinishedCalls: <b>" << countForceFinishedCalls << "</b><br/>\n";
}

bool ThreadCheckStartTable::hasFullHtml() {
    return true;
}
