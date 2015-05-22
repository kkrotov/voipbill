#include "ThreadCurrentCalls.h"
#include "../classes/Billing.h"


void ThreadCurrentCalls::run() {
    currentCalls->currentCdr.load(&db_calls);

    Billing billing;
    billing.calcCurrentCalls();


    auto currentCdr = currentCalls->currentCdr.get();
    int callPerSecond = 0;
    size_t count = currentCdr->size();
    if (count > 0) {
        for (size_t i = count; i > 0; i--) {
            Cdr *cdr = currentCdr->get(i - 1);
            if (cdr->call_id > lastCallId) {
                callPerSecond += 1;
            }
        }
        lastCallId = currentCdr->get(count - 1)->call_id;
    }
    this->callPerSecond = callPerSecond;

    if (callPerSecond > maxCallPerSecond) {
        maxCallPerSecond = callPerSecond;
    }
}

void ThreadCurrentCalls::htmlfull(stringstream &html) {

    this->html(html);

    shared_ptr<CurrentCdrList> cdrList = currentCalls->currentCdr.get();
    if (cdrList == nullptr) {
        return;
    }

    html << "<table width=100% border=0 cellspacing=0>\n";
    html << "<tr>\n";
    html << "<td>Current calls: <b>" << cdrList->size() << "</b>" << "</td>\n";
    html << "<td>Call per second: <b>" << callPerSecond << "</b>" << "</td>\n";
    html << "<td>Max call per second: <b>" << maxCallPerSecond << "</b>" << "</td>\n";
    html << "</td>\n";
    html << "</tr>\n";
    html << "</table>\n";

    html << "<br/>\n";

    auto calls = currentCalls->getCallsWaitingSaving();
    bool fullMode = calls->size() == cdrList->size() * 2;


    html << "<table width=100% border=1 cellspacing=0>\n";
    html << "<tr>";
    if (fullMode) {
        html << "<th nowrap rowspan=2>&nbsp;</th>";
    }
    html << "<th nowrap rowspan=2 colspan=2>connect_time</th>";
    if (fullMode) {
        html << "<th nowrap>src number</th>";
        html << "<th nowrap rowspan=2>billed_time</th>";
        html << "<th nowrap rowspan=2>rate</th>";
        html << "<th nowrap rowspan=2>cost</th>";
        html << "<th nowrap rowspan=2>trunk</th>";
        html << "<th nowrap rowspan=2>account</th>";
        html << "<th nowrap rowspan=2>service</th>";
        html << "<th nowrap rowspan=2>pricelist</th>";
        html << "<th nowrap rowspan=2>prefix</th>";
        html << "<th nowrap rowspan=2>dest</th>";
        html << "<th nowrap rowspan=2>geo_id</th>";
    }
    html << "<th nowrap>src / redirect</th>";
    html << "<th nowrap>noa</th>";
    html << "<th nowrap>src_route</th>";
    html << "</tr>";
    html << "<tr>";
    if (fullMode) {
        html << "<th nowrap>dst number</th>";
    }
    html << "<th nowrap>dst_number</th>";
    html << "<th nowrap>noa</th>";
    html << "<th nowrap>dst_route</th>";
    html << "</tr>\n";

    Call * callOrig = nullptr;
    Call * callTerm = nullptr;

    for (size_t i = 0; i < cdrList->size(); i++) {
        Cdr * cdr = cdrList->get(i);
        if (fullMode) {
            callOrig = &calls->at(i * 2);
            callTerm = &calls->at(i * 2 + 1);
        }

        html << "<tr>";
        if (fullMode) {
            html << "<td nowrap><b>orig</b><br/><b>term</b></td>";
        }
        html << "<td nowrap>" << string_time(cdr->connect_time) << "<br/>" << string_time(cdr->connect_time) << "</td>";
        html << "<td nowrap>" << cdr->session_time << "<br/>" << cdr->session_time << "</td>";
        if (fullMode) {
            string origService("&nbsp;");
            string termService("&nbsp;");
            if (callOrig->trunk_service_id != 0) {
                origService = "t " + lexical_cast<string>(callOrig->trunk_service_id);
            } else if (callOrig->number_service_id != 0) {
                origService = "n " + lexical_cast<string>(callOrig->number_service_id);
            }
            if (callTerm->trunk_service_id != 0) {
                termService = "t " + lexical_cast<string>(callTerm->trunk_service_id);
            } else if (callTerm->number_service_id != 0) {
                termService = "n " + lexical_cast<string>(callTerm->number_service_id);
            }

            html << "<td nowrap>" << callOrig->src_number << "<br/>" << callTerm->dst_number << "</td>";
            html << "<td nowrap>" << callOrig->billed_time << "<br/>" << callTerm->billed_time << "</td>";
            html << "<td nowrap>" << callOrig->rate << "<br/>" << callTerm->rate << "</td>";
            html << "<td nowrap>" << callOrig->cost << "<br/>" << callTerm->cost << "</td>";
            html << "<td nowrap>" << callOrig->trunk_id << "<br/>" << callTerm->trunk_id << "</td>";
            html << "<td nowrap>" << callOrig->account_id << "<br/>" << callTerm->account_id << "</td>";
            html << "<td nowrap>" << origService << "<br/>" << termService << "</td>";
            html << "<td nowrap>" << callOrig->pricelist_id << "<br/>" << callTerm->pricelist_id << "</td>";
            html << "<td nowrap>" << callOrig->prefix << "<br/>" << callTerm->prefix << "</td>";
            html << "<td nowrap>" << callOrig->destination_id << (callOrig->mob ? " mob" : "") << "<br/>" << callTerm->account_id << (callTerm->mob ? " mob" : "") << "</td>";
            html << "<td nowrap>" << callOrig->geo_id << "<br/>" << callTerm->geo_id << "</td>";
        }

        if (strlen(cdr->redirect_number) > 0) {
            html << "<td nowrap>" << cdr->src_number << " / " << cdr->redirect_number[0] << "<br/>" << cdr->dst_number << "</td>";
        } else {
            html << "<td nowrap>" << cdr->src_number << "<br/>" << cdr->dst_number << "</td>";
        }
        html << "<td nowrap>" << cdr->src_noa << "<br/>" << cdr->dst_noa << "</td>";
        html << "<td nowrap>" << cdr->src_route << "<br/>" << cdr->dst_route << "</td>";

        html << "</tr>\n";
    }
    html << "</table>\n";

}

ThreadCurrentCalls::ThreadCurrentCalls() {
    id = idName();
    name = "Current Calls";
    db_calls.setCS(app().conf.db_calls);
    data = DataContainer::instance();
    currentCalls = DataCurrentCallsContainer::instance();
}


