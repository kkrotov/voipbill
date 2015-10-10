#pragma once

#include "BasePage.h"
#include "../classes/Repository.h"

class PageCalls : public BasePage {
public:
    bool canHandle(std::string &path) {
        return path == "/calls";
    }
    void render(std::stringstream &html, map<string, string> &parameters) {
        renderHeader(html);

        Repository repository;

        shared_ptr<CurrentCdrList> cdrList = repository.currentCalls->currentCdr.get();
        if (cdrList == nullptr) {
            return;
        }

        html << "<table width=100% border=0 cellspacing=0>\n";
        html << "<tr>\n";
        html << "<td>Current calls: <b>" << cdrList->size() << "</b>" << "</td>\n";
        html << "<td>Call per second: <b>" << repository.currentCalls->callPerSecond << "</b>" << "</td>\n";
        html << "<td>Max call per second: <b>" << repository.currentCalls->maxCallPerSecond << "</b>" << "</td>\n";
        html << "</td>\n";
        html << "</tr>\n";
        html << "</table>\n";

        html << "<br/>\n";

        auto calls = repository.currentCalls->getCallsWaitingSaving();
        bool fullMode = calls->size() == cdrList->size() * 2;

        html << "<style>\n";
        html << ".orig { color: #006000; }\n";
        html << ".term { color: #000060; }\n";
        html << ".tr_orig td { border-top: 1px solid silver; padding-top: 5px; }\n";
        html << ".tr_term td { padding-bottom: 5px; }\n";
        html << "</style>\n";


        html << "<table width=100% border=0 cellspacing=0>\n";
        html << "<tr>\n";
        html << "<th nowrap rowspan=2 style='text-align: left'>connect_time</th>\n";
        html << "<th nowrap rowspan=2>len</th>\n";
        if (fullMode) {
            html << "<th nowrap rowspan=2>&nbsp;</th>\n";
            html << "<th nowrap rowspan=2>&nbsp;</th>\n";
        }
        if (fullMode) {
            html << "<th nowrap>src number</th>\n";
            html << "<th nowrap rowspan=2>billed_time</th>\n";
            html << "<th nowrap rowspan=2>rate</th>\n";
            html << "<th nowrap rowspan=2>cost</th>\n";
            html << "<th nowrap rowspan=2>trunk</th>\n";
            html << "<th nowrap rowspan=2>account</th>\n";
            html << "<th nowrap rowspan=2>service</th>\n";
            html << "<th nowrap rowspan=2>pricelist</th>\n";
            html << "<th nowrap rowspan=2>prefix</th>\n";
            html << "<th nowrap rowspan=2>dest</th>\n";
            html << "<th nowrap rowspan=2>geo_id</th>\n";
        }
        html << "<th nowrap>src_route</th>\n";
        html << "<th nowrap rowspan=2>call_id</th>\n";
        html << "<th nowrap>src / redirect</th>\n";
        html << "<th nowrap>noa</th>\n";
        html << "</tr>\n";
        html << "<tr>\n";
        if (fullMode) {
            html << "<th nowrap>dst number</th>\n";
        }
        html << "<th nowrap>dst_number</th>\n";
        html << "<th nowrap>noa</th>\n";
        html << "<th nowrap>dst_route</th>\n";
        html << "</tr>\n";

        Call * callOrig = nullptr;
        Call * callTerm = nullptr;

        for (size_t i = 0; i < cdrList->size(); i++) {
            Cdr * cdr = cdrList->get(i);
            if (fullMode) {
                callOrig = &calls->at(i * 2);
                callTerm = &calls->at(i * 2 + 1);
            }

            html << "<tr class='tr_orig'>\n";
            html << "<td nowrap rowspan=2 style='text-align: left'>" << string_time(cdr->connect_time) << "</td>\n";
            html << "<td nowrap rowspan=2>" << cdr->session_time << "</td>\n";
            if (fullMode) {
                html << "<td nowrap><a class=orig href='/test/calc/gui?call_id=" << cdr->call_id << "&orig=t'>orig</a></td>\n";
                html << "<td nowrap>" << (callOrig->our ? "client" : "operator") << "</td>\n";
            }
            if (fullMode) {
                string origService("<span style='color: red'>unknown</span>");
                if (callOrig->trunk_service_id != 0) {
                    origService = "t " + lexical_cast<string>(callOrig->trunk_service_id);
                } else if (callOrig->number_service_id != 0) {
                    origService = "n " + lexical_cast<string>(callOrig->number_service_id);
                }

                html << "<td nowrap class=orig>" << callOrig->src_number << "</td>\n";
                html << "<td nowrap class=orig>" << callOrig->billed_time << "</td>\n";
                html << "<td nowrap class=orig>" << callOrig->rate << "</td>\n";
                html << "<td nowrap class=orig>" << callOrig->cost << "</td>\n";
                html << "<td nowrap class=orig>" << callOrig->trunk_id << "</td>\n";
                html << "<td nowrap><a class=orig href='/client?id=" << callOrig->account_id << "'>" << callOrig->account_id << "</a></td>\n";
                html << "<td nowrap class=orig>" << origService << "</td>\n";
                html << "<td nowrap class=orig>" << callOrig->pricelist_id << "</td>\n";
                html << "<td nowrap class=orig>" << callOrig->prefix << "</td>\n";
                html << "<td nowrap class=orig>" << callOrig->destination_id << (callOrig->mob ? " mob" : "") << "</td>\n";
                html << "<td nowrap class=orig>" << callOrig->geo_id << "</td>\n";
            }

            html << "<td nowrap class=orig>" << cdr->src_route << "</td>\n";
            html << "<td nowrap rowspan=2>" << cdr->call_id << "</td>\n";
            if (strlen(cdr->redirect_number) > 0) {
                html << "<td nowrap class=orig>" << cdr->src_number << " / " << cdr->redirect_number[0] << "</td>\n";
            } else {
                html << "<td nowrap class=orig>" << cdr->src_number << "</td>\n";
            }
            html << "<td nowrap class=orig>" << cdr->src_noa << "</td>\n";

            html << "</tr>\n";

            html << "<tr class='tr_term'>\n";
            if (fullMode) {
                html << "<td nowrap><a class=term href='/test/calc/gui?call_id=" << cdr->call_id << "&orig=f'>term</a></td>\n";
                html << "<td nowrap>" << (callTerm->our ? "client" : "operator") << "</td>\n";
            }
            if (fullMode) {
                string termService("<span style='color: red'>unknown</span>");
                if (callTerm->trunk_service_id != 0) {
                    termService = "t " + lexical_cast<string>(callTerm->trunk_service_id);
                } else if (callTerm->number_service_id != 0) {
                    termService = "n " + lexical_cast<string>(callTerm->number_service_id);
                }

                html << "<td nowrap class=term>" << callTerm->dst_number << "</td>\n";
                html << "<td nowrap class=term>" << callTerm->billed_time << "</td>\n";
                html << "<td nowrap class=term>" << callTerm->rate << "</td>\n";
                html << "<td nowrap class=term>" << callTerm->cost << "</td>\n";
                html << "<td nowrap class=term>" << callTerm->trunk_id << "</td>\n";
                html << "<td nowrap><a class=term href='/client?id=" << callTerm->account_id << "'>" << callTerm->account_id << "</a></td>\n";
                html << "<td nowrap class=term>" << termService << "</td>\n";
                html << "<td nowrap class=term>" << callTerm->pricelist_id << "</td>\n";
                html << "<td nowrap class=term>" << callTerm->prefix << "</td>\n";
                html << "<td nowrap class=term>" << callTerm->account_id << (callTerm->mob ? " mob" : "") << "</td>\n";
                html << "<td nowrap class=term>" << callTerm->geo_id << "</td>\n";
            }

            html << "<td nowrap class=term>" << cdr->dst_route << "</td>\n";
            if (strlen(cdr->redirect_number) > 0) {
                html << "<td nowrap class=term>" << cdr->dst_number << "</td>\n";
            } else {
                html << "<td nowrap class=term>" << cdr->dst_number << "</td>\n";
            }
            html << "<td nowrap class=term>" << cdr->dst_noa << "</td>\n";

            html << "</tr>\n";
        }
        html << "</table>\n";


    }
};