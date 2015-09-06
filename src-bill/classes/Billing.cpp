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
    auto statsAccount = shared_ptr<StatsAccountManager>(new StatsAccountManager());
    auto statsFreemin = shared_ptr<StatsFreeminManager>(new StatsFreeminManager());
    auto statsPackage = shared_ptr<StatsPackageManager>(new StatsPackageManager());

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

        updateClientCounters(origCallInfo, statsAccount.get());
        updateClientCounters(termCallInfo, statsAccount.get());
        updateFreeMinsCounters(origCallInfo, statsFreemin.get());
        updatePackageStats(origCallInfo, statsPackage.get());
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

    if (repository.billingData->calls.size() >= calls_max_queue_length) {
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

            updateClientCounters(origCallInfo, &repository.billingData->statsAccount);
            updateClientCounters(termCallInfo, &repository.billingData->statsAccount);
            updateFreeMinsCounters(origCallInfo, &repository.billingData->statsFreemin);
            updatePackageStats(origCallInfo, &repository.billingData->statsPackage);

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

void Billing::updateClientCounters(CallInfo &callInfo, StatsAccountManager *statsAccount) {
    statsAccount->add(&callInfo);
}

void Billing::updateFreeMinsCounters(CallInfo &callInfo, StatsFreeminManager *statsFreemin) {
    statsFreemin->add(callInfo.call);
}

void Billing::updatePackageStats(CallInfo &callInfo, StatsPackageManager *statsPackage) {
    statsPackage->add(&callInfo);
}

