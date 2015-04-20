#include "Billing.h"
#include "BillingCall.h"


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

void Billing::setDstBillingData(DataBillingContainer *billingData) {
    this->dstBillingData = billingData;
}

void Billing::calcCurrentCdrs() {
    BillingCall billingCall(this);
    auto currentCdrs = data->currentCdrData.get();

    time_t now = time(NULL);
    PreparedData preparedData;

    for (size_t i = 0; i < currentCdrs->size(); i++) {
        auto cdr = currentCdrs->get(i);
        time_t cdr_time = parseDateTime(cdr->connect_time);

        if (!data->prepareData(preparedData, cdr_time)) {
            break;
        }

        cdr->session_time = (int)(now - cdr_time);

        Call origCall = Call(cdr, CALL_ORIG);
        billingCall.calc(&origCall, cdr, &preparedData);

        Call termCall = Call(cdr, CALL_TERM);
        billingCall.calc(&termCall, cdr, &preparedData);

        dstBillingData->calls.push_back(origCall);
        dstBillingData->calls.push_back(termCall);
    }
}

void Billing::calcCdrs() {

}


void Billing::calc(Call &call) {

}


