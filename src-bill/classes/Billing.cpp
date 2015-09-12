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
    const int calls_max_queue_length = 1000000;

    if (!repository.billingData->ready()) {
        return;
    }

    if (repository.billingData->callsQueueSize() >= calls_max_queue_length) {
        return;
    }

    BillingCall billingCall(&repository);

    bool calcLoop = true;
    Cdr cdr;
    while (calcLoop) {
        if (!repository.billingData->cdrs.get(cdr)) {
            break;
        }

        try {

            if (!repository.prepare(cdr.connect_time)) {
                repository.billingData->cdrs.revert(cdr);
                break;
            }

            long long int lastCallId = repository.billingData->getCallsLastId();

            Call origCall = Call(&cdr, CALL_ORIG);
            Call termCall = Call(&cdr, CALL_TERM);

            CallInfo origCallInfo;
            CallInfo termCallInfo;

            origCall.id = lastCallId + 1;
            origCall.peer_id = lastCallId + 2;
            termCall.id = lastCallId + 2;
            termCall.peer_id = lastCallId + 1;

            billingCall.calc(&origCall, &origCallInfo, &cdr);

            termCall.src_number = origCall.src_number;
            termCall.dst_number = origCall.dst_number;
            billingCall.calc(&termCall, &termCallInfo, &cdr);

            repository.billingData->addCall(&origCallInfo);
            repository.billingData->addCall(&termCallInfo);

            if (repository.billingData->callsQueueSize() >= calls_max_queue_length) {
                calcLoop = false;
            }

        } catch (Exception &e) {
            e.addTrace("Billing::calc");
            repository.billingData->cdrs.revert(cdr);
            throw e;
        } catch (std::exception &e) {
            repository.billingData->cdrs.revert(cdr);
            throw e;
        } catch (...) {
            repository.billingData->cdrs.revert(cdr);
            throw Exception("Unknown error", "Billing::calc");
        }
    }

}
