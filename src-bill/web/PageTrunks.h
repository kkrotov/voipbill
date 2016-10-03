#pragma once

#include "BasePage.h"
#include "../classes/Repository.h"

class TrunkIoParams {

public:
    string trunk_name;
    int num_of_incoming,
        num_of_outgoing;
    double incoming_cost,
        outgoing_cost;

    TrunkIoParams (string name, bool incoming, double cost) {

        this->trunk_name = name;
        if (incoming) {

            this->num_of_outgoing = 0;
            this->num_of_incoming = 1;
            incoming_cost = cost;
            outgoing_cost = 0;
        }
        else {

            this->num_of_outgoing = 1;
            this->num_of_incoming = 0;
            incoming_cost = 0;
            outgoing_cost = cost;
        }
    }
};

class ActiveTrunks {

    vector<TrunkIoParams> activeTrunks;

    void add (string name, bool incoming, double cost) {

        for (TrunkIoParams &trunk: activeTrunks) {

            if (trunk.trunk_name.compare(name)==0) {

                if (incoming) {

                    trunk.num_of_incoming++;
                    trunk.incoming_cost += cost;
                }
                else {

                    trunk.num_of_outgoing++;
                    trunk.outgoing_cost += cost;
                }
                return;
            }
        }
        activeTrunks.push_back(TrunkIoParams(name, incoming, cost));
    }

public:
    ActiveTrunks (shared_ptr<vector<Call>> calls, shared_ptr<CurrentCdrList> cdrList) {

        bool fullMode = calls->size() == cdrList->size() * 2;
        for (size_t i = 0; i < cdrList->size(); i++) {

            Cdr *cdr = cdrList->get(i);
            double orig_cost=0, term_cost=0;
            if (fullMode) {

                Call *callOrig = &calls->at(i * 2);
                orig_cost = callOrig->cost;
                Call *callTerm = &calls->at(i * 2 + 1);
                term_cost = callTerm->cost;
            }
            add(cdr->src_route, true, orig_cost);
            add(cdr->dst_route, false, term_cost);
        }
    }

    int count() { return activeTrunks.size(); }
    TrunkIoParams at(int i) { return activeTrunks.at(i); }
};

class PageTrunks : public BasePage {

    Repository repository;

public:
    bool canHandle(std::string &path) {
        return path == "/trunks";
    }
    void render(std::stringstream &html, map<string, string> &parameters) {

        if (!ready())
            return;

        renderHeader(html);

        shared_ptr<CurrentCdrList> cdrList = repository.currentCalls->currentCdr.get();
        if (cdrList == nullptr) {
            return;
        }
        shared_ptr<vector<Call>> calls = repository.currentCalls->getCallsWaitingSaving();
        ActiveTrunks activeTrunks(calls, cdrList);

        html << "<table width=100% border=0 cellspacing=0>\n";
        html << "<tr>\n";
        html << "<td>Active trunks: <b>" << activeTrunks.count() << "</b>" << "</td>\n";
        html << "<td>Current calls: <b>" << cdrList->size() << "</b>" << "</td>\n";
        html << "</td>\n";
        html << "</tr>\n";
        html << "</table>\n";
        html << "<br/>\n";

        html << "<style>\n";
        html << ".orig { color: #006000; }\n";
        html << ".term { color: #000060; }\n";
        html << ".tr_orig td { border-top: 1px solid silver; padding-top: 5px; }\n";
        html << ".tr_term td { padding-bottom: 5px; }\n";
        html << "</style>\n";

        bool fullMode = calls->size() == cdrList->size() * 2;
        html << "<table width=100% border=0 cellspacing=0>\n";
        html << "<tr>"
                "<th nowrap>trunk_name</th>"
                "<th nowrap>capacity</th>"
                "<th nowrap>incoming count</th>";
        if (fullMode)
            html << "<th nowrap>incoming cost</th>";

        html << "<th nowrap>outgoing count</th>";
        if (fullMode)
            html << "<th nowrap>outgoing cost</th>";

        html << "<th nowrap>total count</th>";
        if (fullMode)
            html << "<th nowrap>total cost</th>";

        html << "<th nowrap>load</th>";
        html << "</tr>";;

        for (size_t i = 0; i < activeTrunks.count(); i++) {

            TrunkIoParams trunkParam = activeTrunks.at(i);
            html << "<tr class='tr_orig'>\n";
            html << "<td nowrap><a class=orig href='/calls?trunkname=" << trunkParam.trunk_name << "'>" << trunkParam.trunk_name << "</a></td>\n";

            Trunk *trunk = repository.getTrunkByName(trunkParam.trunk_name.c_str());
            int capacity = trunk!= nullptr? trunk->capacity:0;
            html << "<td nowrap>" << capacity << "</td>\n";

            if (trunkParam.num_of_incoming>0)
                html << "<td nowrap><a class=orig href='/calls?trunkname=" << trunkParam.trunk_name << "&dest=in'>" << trunkParam.num_of_incoming << "</a></td>\n";
            else
                html << "<td nowrap>" << trunkParam.num_of_incoming << "</td>\n";

            if (fullMode)
                html << "<td nowrap>" << trunkParam.incoming_cost << "</td>\n";

            if (trunkParam.num_of_outgoing>0)
                html << "<td nowrap><a class=term href='/calls?trunkname=" << trunkParam.trunk_name << "&dest=out'>" << trunkParam.num_of_outgoing << "</a></td>\n";
            else
                html << "<td nowrap>" << trunkParam.num_of_outgoing << "</td>\n";

            if (fullMode)
                html << "<td nowrap>" << trunkParam.outgoing_cost << "</td>\n";

            html << "<td nowrap><a class=orig href='/calls?trunkname=" << trunkParam.trunk_name << "'>" << trunkParam.num_of_incoming+trunkParam.num_of_outgoing << "</a></td>\n";
            if (fullMode)
                html << "<td nowrap>" << trunkParam.incoming_cost+trunkParam.outgoing_cost << "</td>\n";

            string load = capacity>0? (trunkParam.num_of_incoming+trunkParam.num_of_outgoing)*100/capacity+"%":" ";
            html << "<td nowrap>" <<  load << "</td>\n";

            html << "</tr>\n";
        }
        html << "</table>\n";
    }

    bool ready() {

        if (!repository.billingData->ready())
            return false;

        return repository.prepare();
    }
};