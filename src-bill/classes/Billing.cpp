#include "Billing.h"
#include "BillingCall.h"

void Billing::setData(DataContainer *data) {
    repository.data = data;
}

void Billing::setBillingData(DataBillingContainer *billingData) {
    repository.billingData = billingData;
}

void Billing::calcCurrentCalls() {

    if (!repository.currentCalls->ready()) {
        return;
    }

    BillingCall billingCall(&repository);

    auto currentCdrs = repository.currentCalls->currentCdr.get();

    shared_ptr<vector<Call>> callsWaitSaving(new vector<Call>());
    shared_ptr<StatsAccountManager> statsAccount(new StatsAccountManager());
    shared_ptr<StatsFreeminManager> statsFreemin(new StatsFreeminManager());
    shared_ptr<StatsPackageManager> statsPackage(new StatsPackageManager());

    for (size_t i = 0; i < currentCdrs->size(); i++) {
        auto cdr = currentCdrs->get(i);

        if (!repository.prepare(cdr->connect_time)) {
            break;
        }

        Call origCall = Call(cdr, CALL_ORIG);
        Call termCall = Call(cdr, CALL_TERM);

        CallInfo origCallInfo;
        CallInfo termCallInfo;

        billingCall.calc(&origCall, &origCallInfo, cdr);

        termCall.src_number = origCall.src_number;
        termCall.dst_number = origCall.dst_number;
        billingCall.calc(&termCall, &termCallInfo, cdr);

        callsWaitSaving->push_back(origCall);
        statsAccount.get()->add(&origCallInfo);
        statsFreemin.get()->add(&origCallInfo);
        statsPackage.get()->add(&origCallInfo);

        callsWaitSaving->push_back(termCall);
        statsAccount.get()->add(&termCallInfo);
        statsFreemin.get()->add(&termCallInfo);
        statsPackage.get()->add(&termCallInfo);
    }

    repository.currentCalls->setCallsWaitingSaving(callsWaitSaving);
    repository.currentCalls->setStatsAccount(statsAccount);
    repository.currentCalls->setStatsFreemin(statsFreemin);
    repository.currentCalls->setStatsPackage(statsPackage);
}

void Billing::calc() {

    if (!repository.billingData->ready()) {
        return;
    }

    BillingCall billingCall(&repository);


    while (true) {

        if (repository.billingData->callsQueueSize() >= CALLS_MAX_QUEUE_SIZE) {
            break;
        }

        Cdr *cdr = repository.billingData->getFirstCdr();
        if (cdr == nullptr) {
            break;
        }

        if (!repository.prepare(cdr->connect_time)) {
            break;
        }

        long long int lastCallId = repository.billingData->getCallsLastId();

        Call origCall = Call(cdr, CALL_ORIG);
        Call termCall = Call(cdr, CALL_TERM);

        CallInfo origCallInfo;
        CallInfo termCallInfo;

        origCall.id = lastCallId + 1;
        origCall.peer_id = lastCallId + 2;
        termCall.id = lastCallId + 2;
        termCall.peer_id = lastCallId + 1;

        billingCall.calc(&origCall, &origCallInfo, cdr);

        termCall.src_number = origCall.src_number;
        termCall.dst_number = origCall.dst_number;
        billingCall.calc(&termCall, &termCallInfo, cdr);

        repository.billingData->addCall(&origCallInfo);
        repository.billingData->addCall(&termCallInfo);

        repository.billingData->removeFirstCdr();
    }

}
