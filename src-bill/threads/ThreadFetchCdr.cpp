#include "ThreadFetchCdr.h"
#include "../classes/AppBill.h"


ThreadFetchCdr::ThreadFetchCdr() {
    id = idName();
    name = "Fetch CDR";

    db_calls.setCS(app().conf.db_calls);
    billingData = DataBillingContainer::instance();

    cdrLoader.setDb(&db_calls);
    cdrLoader.setBillingData(billingData);
}

bool ThreadFetchCdr::prepare() {
    cdrLoader.prepare();
    return true;
}

void ThreadFetchCdr::run() {

    const size_t rows_per_request = 25000;

    while (!cdrLoader.load(rows_per_request)) {
        boost::this_thread::interruption_point();
    }

}

void ThreadFetchCdr::htmlfull(stringstream &html) {
    this->html(html);
    html << "last cdr id: <b>" << cdrLoader.lastCdrId << "</b><br/>\n";
    DataBillingContainer::instance()->html(html);
}
