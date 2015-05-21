#include "Billing.h"
#include "BillingCall.h"
#include "../data/DataCurrentCallsContainer.h"


Billing::Billing() {
    data = DataContainer::instance();
    billingData = DataBillingContainer::instance();
}

void Billing::setData(DataContainer *data) {
    this->data = data;
}

void Billing::setBillingData(DataBillingContainer *billingData) {
    this->billingData = billingData;
}

void Billing::calcCurrentCalls() {

    auto dataCurrentCalls = DataCurrentCallsContainer::instance();
    if (!dataCurrentCalls->ready()) {
        return;
    }

    BillingCall billingCall(this);

    auto currentCdrs = dataCurrentCalls->currentCdr.get();

    PreparedData preparedData;

    auto callsWaitSaving = shared_ptr<vector<Call>>(new vector<Call>());
    auto clientCounter = shared_ptr<ClientCounter>(new ClientCounter());
    auto fminCounter = shared_ptr<FminCounter>(new FminCounter());

    for (size_t i = 0; i < currentCdrs->size(); i++) {
        auto cdr = currentCdrs->get(i);

        if (!data->prepareData(preparedData, cdr->connect_time)) {
            break;
        }

        Call origCall = Call(cdr, CALL_ORIG);
        billingCall.calc(&origCall, cdr, &preparedData);

        Call termCall = Call(cdr, CALL_TERM);
        termCall.src_number = origCall.src_number;
        termCall.dst_number = origCall.dst_number;
        billingCall.calc(&termCall, cdr, &preparedData);

        callsWaitSaving->push_back(origCall);
        callsWaitSaving->push_back(termCall);

        updateClientCounters(origCall, preparedData, clientCounter.get());
        updateClientCounters(termCall, preparedData, clientCounter.get());
        updateFreeMinsCounters(origCall, fminCounter.get());
    }

    dataCurrentCalls->setCallsWaitingSaving(callsWaitSaving);
    dataCurrentCalls->setClientCounter(clientCounter);
    dataCurrentCalls->setFminCounter(fminCounter);
}

void Billing::calc() {
    const int calls_max_queue_length = 1000000;

    if (!billingData->ready()) {
        return;
    }

    {
        lock_guard<Spinlock> guard(billingData->callsWaitSavingLock);
        if (billingData->callsWaitSaving.size() >= calls_max_queue_length) {
            return;
        }
    }

    BillingCall billingCall(this);

    PreparedData preparedData;

    auto fminCounter = billingData->fminCounter.get();
    auto clientCounter = billingData->clientCounter.get();

    bool calcLoop = true;
    Cdr cdr;
    while (calcLoop) {
        {
            lock_guard<Spinlock> guard(billingData->cdrsWaitProcessingLock);
            if (billingData->cdrsWaitProcessing.size() == 0) {
                break;
            }
            cdr = billingData->cdrsWaitProcessing.front();
            billingData->cdrsWaitProcessing.pop_front();
        }

        try {


            if (!data->prepareData(preparedData, cdr.connect_time)) {
                lock_guard<Spinlock> guard(billingData->cdrsWaitProcessingLock);
                billingData->cdrsWaitProcessing.push_front(cdr);
                break;
            }

            Call origCall = Call(&cdr, CALL_ORIG);
            origCall.id = billingData->lastCallId + 1;
            origCall.peer_id = billingData->lastCallId + 2;
            billingCall.calc(&origCall, &cdr, &preparedData);

            Call termCall = Call(&cdr, CALL_TERM);
            termCall.src_number = origCall.src_number;
            termCall.dst_number = origCall.dst_number;
            termCall.id = billingData->lastCallId + 2;
            termCall.peer_id = billingData->lastCallId + 1;
            billingCall.calc(&termCall, &cdr, &preparedData);

            billingData->calcedCdrsCount += 1;
            billingData->lastCallId += 2;
            billingData->lastTime = origCall.connect_time;

            updateClientCounters(origCall, preparedData, clientCounter.get());
            updateClientCounters(termCall, preparedData, clientCounter.get());
            updateFreeMinsCounters(origCall, fminCounter.get());

            {
                lock_guard<Spinlock> guard(billingData->callsWaitSavingLock);
                billingData->callsWaitSaving.push_back(origCall);
                billingData->callsWaitSaving.push_back(termCall);

                if (billingData->callsWaitSaving.size() >= calls_max_queue_length) {
                    calcLoop = false;
                }
            }

        } catch (Exception &e) {
            e.addTrace("Billing::calc");
            lock_guard<Spinlock> guard(billingData->cdrsWaitProcessingLock);
            billingData->cdrsWaitProcessing.push_front(cdr);
            throw e;
        } catch (std::exception &e) {
            lock_guard<Spinlock> guard(billingData->cdrsWaitProcessingLock);
            billingData->cdrsWaitProcessing.push_front(cdr);
            throw e;
        } catch (...) {
            lock_guard<Spinlock> guard(billingData->cdrsWaitProcessingLock);
            billingData->cdrsWaitProcessing.push_front(cdr);
            throw Exception("Unknown error", "Billing::calc");
        }
    }

}

void Billing::updateClientCounters(Call &call, PreparedData &preparedData, ClientCounter * clientCounter) {
    if (abs(call.cost) < 0.000001) {
        return;
    }


    ClientCounterObj &cc = clientCounter->counter[call.account_id];

    if (abs(call.dt.month - cc.amount_month) < 43200) {
        cc.amount_month = call.dt.month;
        cc.sum_month += call.cost;
    } else if (call.dt.month > cc.amount_month) {
        cc.amount_month = call.dt.month;
        cc.sum_month = call.cost;
    }

    if (abs(call.dt.day - cc.amount_day) < 43200) {
        cc.amount_day = call.dt.day;
        cc.sum_day += call.cost;
    } else if (call.dt.day > cc.amount_day) {
        cc.amount_day = call.dt.day;
        cc.sum_day = call.cost;
    }

    auto client = preparedData.client->find(call.account_id);
    if (client != nullptr && call.connect_time >= client->amount_date) {
        cc.sum += call.cost;
    }

    if (cc.client_id == 0) {
        cc.client_id = call.account_id;
    }

    cc.updated = 1;

}

void Billing::updateFreeMinsCounters(Call &call, FminCounter * fminCounter) {
    if (call.service_package_id != 0 && call.number_service_id != 0) {
        fminCounter->add(call.number_service_id, 1, call.dt.month, call.package_time);
    }
}

