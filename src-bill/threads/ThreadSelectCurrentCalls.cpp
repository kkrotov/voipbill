#include "ThreadSelectCurrentCalls.h"
#include "../classes/Billing.h"


void ThreadSelectCurrentCalls::run() {

    data->currentCdrData.load(&db_calls);
}

void ThreadSelectCurrentCalls::htmlfull(stringstream &html) {

    this->html(html);

    if (!data->currentCdrData.ready()) {
        return;
    }

    shared_ptr<CurrentCdrList> cdrList = data->currentCdrData.get();

    DataBillingContainer dstBillingData;
    Billing billing;
    billing.setDstBillingData(&dstBillingData);
    billing.calcCurrentCdrs();

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
        html << "<td>" << cdr->connect_time << "</td>";
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
    html << "<th>call_time</th>";
    html << "<th>src_number</th>";
    html << "<th>dst_number</th>";
    html << "<th>redirect_number</th>";
    html << "<th>billed_time</th>";
    html << "<th>rate</th>";
    html << "<th>cost</th>";
    html << "<th>client</th>";
    html << "<th>service</th>";
    html << "<th>pricelist_id</th>";
    html << "<th>prefix</th>";
    html << "<th>dest</th>";
    html << "<th>geo_id</th>";
    html << "</tr>\n";
    for (auto &call : dstBillingData.calls) {
        html << "<tr>";
        html << "<td>" << (call.orig ? "orig" : "term") << "</td>";
        html << "<td>" << call.call_time << "</td>";
        html << "<td>" << call.src_number << "</td>";
        html << "<td>" << call.dst_number << "</td>";
        html << "<td>" << call.redirect_number << "</td>";
        html << "<td>" << call.billed_time << "</td>";
        html << "<td>" << call.rate << "</td>";
        html << "<td>" << call.cost << "</td>";
        html << "<td>" << call.client_account_id << "</td>";
        html << "<td>" << (call.service_type == SERVICE_TYPE_TRUNK ? "trunk " : "") << (call.service_type == SERVICE_TYPE_NUMBER ? "number " : "") << call.service_id << "</td>";
        html << "<td>" << call.pricelist_id << "</td>";
        html << "<td>" << call.prefix << "</td>";
        html << "<td>" << call.dest << (call.mob ? " mob" : "") << "</td>";
        html << "<td>" << call.geo_id << "</td>";
        html << "</tr>\n";
    }
    html << "</table>\n";
}

ThreadSelectCurrentCalls::ThreadSelectCurrentCalls() {
    id = idName();
    name = "Current Calls";
    db_calls.setCS(app().conf.db_calls);
    data = DataContainer::instance();
}


