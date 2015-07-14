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

    auto callsWaitSaving = shared_ptr<vector<Call>>(new vector<Call>());
    auto clientCounter = shared_ptr<ClientCounter>(new ClientCounter());
    auto fminCounter = shared_ptr<FminCounter>(new FminCounter());
    auto statsPackagesCounter = shared_ptr<StatsPackageCounter>(new StatsPackageCounter());

    for (size_t i = 0; i < currentCdrs->size(); i++) {
        auto cdr = currentCdrs->get(i);

        if (!repository.prepare(cdr->connect_time)) {
            break;
        }

        Call origCall = Call(cdr, CALL_ORIG);
        CallInfo origCallInfo;
        billingCall.calc(&origCall, &origCallInfo, cdr);

        Call termCall = Call(cdr, CALL_TERM);
        CallInfo termCallInfo;
        termCall.src_number = origCall.src_number;
        termCall.dst_number = origCall.dst_number;
        billingCall.calc(&termCall, &termCallInfo, cdr);

        callsWaitSaving->push_back(origCall);
        callsWaitSaving->push_back(termCall);

        updateClientCounters(origCallInfo, clientCounter.get());
        updateClientCounters(termCallInfo, clientCounter.get());
        updateFreeMinsCounters(origCallInfo, fminCounter.get());
        updatePackageStats(origCallInfo, statsPackagesCounter.get());
    }

    repository.currentCalls->setCallsWaitingSaving(callsWaitSaving);
    repository.currentCalls->setClientCounter(clientCounter);
    repository.currentCalls->setFminCounter(fminCounter);
    repository.currentCalls->setStatsPackagesCounter(statsPackagesCounter);
}

void Billing::calc() {
    const int calls_max_queue_length = 1000000;

    if (!repository.billingData->ready()) {
        return;
    }

    if (repository.billingData->calls.size() >= calls_max_queue_length) {
        return;
    }

    BillingCall billingCall(&repository);

    auto fminCounter = repository.billingData->fminCounter.get();
    auto clientCounter = repository.billingData->clientCounter.get();
    auto statsPackagesCounter = repository.billingData->statsPackageCounter.get();

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

            long long int lastCallId = repository.billingData->calls.getLastId();

            Call origCall = Call(&cdr, CALL_ORIG);
            CallInfo origCallInfo;
            origCall.id = lastCallId + 1;
            origCall.peer_id = lastCallId + 2;
            billingCall.calc(&origCall, &origCallInfo, &cdr);

            Call termCall = Call(&cdr, CALL_TERM);
            CallInfo termCallInfo;
            termCall.src_number = origCall.src_number;
            termCall.dst_number = origCall.dst_number;
            termCall.id = lastCallId + 2;
            termCall.peer_id = lastCallId + 1;
            billingCall.calc(&termCall, &termCallInfo, &cdr);

            updateClientCounters(origCallInfo, clientCounter.get());
            updateClientCounters(termCallInfo, clientCounter.get());
            updateFreeMinsCounters(origCallInfo, fminCounter.get());
            updatePackageStats(origCallInfo, statsPackagesCounter.get());

            repository.billingData->calls.add(origCall, termCall);

            if (repository.billingData->calls.size() >= calls_max_queue_length) {
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

void Billing::updateClientCounters(CallInfo &callInfo, ClientCounter * clientCounter) {
    clientCounter->add(callInfo.call, callInfo.account);
}

void Billing::updateFreeMinsCounters(CallInfo &callInfo, FminCounter * fminCounter) {
    fminCounter->add(callInfo.call);
}

void Billing::updatePackageStats(CallInfo &callInfo, StatsPackageCounter * statsPackageCounter) {
    statsPackageCounter->add(&callInfo);
}

