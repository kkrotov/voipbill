#include "ThreadCurrentCalls.h"
#include "../classes/Billing.h"
#include "../data/DataCurrentCallsContainer.h"


void ThreadCurrentCalls::run() {
    DataCurrentCallsContainer::instance()->currentCdr.load(&db_calls);

    Billing billing;
    billing.calcCurrentCalls();
}

void ThreadCurrentCalls::htmlfull(stringstream &html) {

    this->html(html);

    shared_ptr<CurrentCdrList> cdrList = DataCurrentCallsContainer::instance()->currentCdr.get();
    if (cdrList == nullptr) {
        return;
    }


    html << "Current calls: <b>" << cdrList->size() << "</b><br/>\n";
    html << "<br/>\n";

    html << "<table width=100% border=1 cellspacing=0>\n";
    html << "<tr>";
    html << "<th>connect_time</th>";
    html << "<th>session_time</th>";
    html << "<th>src_number</th>";
    html << "<th>dst_number</th>";
    html << "<th>redirect_number</th>";
    html << "<th>src_route</th>";
    html << "<th>noa</th>";
    html << "<th>dst_route</th>";
    html << "<th>noa</th>";
    html << "</tr>\n";
    for (size_t i = 0; i < cdrList->size(); i++) {
        auto cdr = cdrList->get(i);

        html << "<tr>";
        html << "<td>" << string_time(cdr->connect_time) << "</td>";
        html << "<td>" << cdr->session_time << "</td>";
        html << "<td>" << cdr->src_number << "</td>";
        html << "<td>" << cdr->dst_number << "</td>";
        html << "<td>" << cdr->redirect_number << "</td>";
        html << "<td>" << cdr->src_route << "</td>";
        html << "<td>" << cdr->src_noa << "</td>";
        html << "<td>" << cdr->dst_route << "</td>";
        html << "<td>" << cdr->dst_noa << "</td>";
        html << "</tr>\n";
    }
    html << "</table>\n";

    html << "<br/><br/>\n";

    html << "<table width=100% border=1 cellspacing=0>\n";
    html << "<tr>";
    html << "<th></th>";
    html << "<th>connect_time</th>";
    html << "<th>src_number</th>";
    html << "<th>dst_number</th>";
    html << "<th>billed_time</th>";
    html << "<th>rate</th>";
    html << "<th>cost</th>";
    html << "<th>account</th>";
    html << "<th>number</th>";
    html << "<th>pricelist_id</th>";
    html << "<th>prefix</th>";
    html << "<th>dest</th>";
    html << "<th>geo_id</th>";
    html << "</tr>\n";
    auto calls = DataCurrentCallsContainer::instance()->getCallsWaitingSaving();
    for (auto &call : *calls.get()) {
        html << "<tr>";
        html << "<td>" << (call.orig ? "orig" : "term") << "</td>";
        html << "<td>" << string_time(call.connect_time) << "</td>";
        html << "<td>" << call.src_number << "</td>";
        html << "<td>" << call.dst_number << "</td>";
        html << "<td>" << call.billed_time << "</td>";
        html << "<td>" << call.rate << "</td>";
        html << "<td>" << call.cost << "</td>";
        html << "<td>" << call.account_id << "</td>";
        if (call.trunk_service_id != 0) {
            html << "<td>trunk " << call.trunk_service_id << "</td>";
        } else if (call.number_service_id != 0) {
            html << "<td>number " << call.number_service_id << "</td>";
        } else {
            html << "<td></td>";
        }
        html << "<td>" << call.pricelist_id << "</td>";
        html << "<td>" << call.prefix << "</td>";
        html << "<td>" << call.dest << (call.mob ? " mob" : "") << "</td>";
        html << "<td>" << call.geo_id << "</td>";
        html << "</tr>\n";
    }
    html << "</table>\n";
}

ThreadCurrentCalls::ThreadCurrentCalls() {
    id = idName();
    name = "Current Calls";
    db_calls.setCS(app().conf.db_calls);
    data = DataContainer::instance();
}


