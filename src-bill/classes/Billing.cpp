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

    for (size_t i = 0; i < currentCdrs->size(); i++) {
        auto cdr = currentCdrs->get(i);
        time_t cdr_time = parseDateTime(cdr->connect_time);
        cdr->session_time = (int)(now - cdr_time);

        Call origCall = Call(cdr, CALL_ORIG);
        if (billingCall.init(&origCall, cdr)) { ;
            billingCall.calc();
        } else {
            break;
        }

        Call termCall = Call(cdr, CALL_TERM);
        if (billingCall.init(&termCall, cdr)) {
            billingCall.calc();
        } else {
            break;
        }
    }
}

void Billing::calcCdrs() {

}


void Billing::calc(Call &call) {

}


