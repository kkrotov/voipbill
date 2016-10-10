#pragma once

#include "BasePage.h"
#include "../classes/Repository.h"

class TrunkIoParams {

public:
    string trunk_name;
    int num_of_incoming,
        num_of_outgoing,
        capacity;
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
    void setCapacity(int capacity) {this->capacity=capacity;};
    int getLoad () { return capacity>0? ((num_of_incoming+num_of_outgoing)*100/capacity):100; };
};

class ActiveTrunks {

    vector<TrunkIoParams> trunkIoParams;

    void add (string name, bool incoming, double cost) {

        for (TrunkIoParams &trunk: trunkIoParams) {

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
        trunkIoParams.push_back(TrunkIoParams(name, incoming, cost));
    }

public:
    ActiveTrunks (Repository &repository) {

        shared_ptr<CurrentCdrList> cdrList = repository.currentCalls->currentCdr.get();
        if (cdrList == nullptr) {
            return;
        }
        shared_ptr<vector<Call>> calls = repository.currentCalls->getCallsWaitingSaving();

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
        for (size_t i = 0; i < this->trunkIoParams.size(); i++) {

            TrunkIoParams trunkParam = this->trunkIoParams.at(i);
            Trunk *trunk = repository.getTrunkByName(trunkParam.trunk_name.c_str());
            trunkParam.capacity = trunk!= nullptr? trunk->capacity:0;
            this->trunkIoParams[i] = trunkParam;
        }
    }

    int count() { return trunkIoParams.size(); }

    void sort(int sort_column, bool order) {

        switch (sort_column) {

            case 0:
                if (order)
                    std::sort(trunkIoParams.begin(), trunkIoParams.end(), [](TrunkIoParams a, TrunkIoParams b) {
                        return b.trunk_name > a.trunk_name;
                    });
                else
                    std::sort(trunkIoParams.begin(), trunkIoParams.end(), [](TrunkIoParams a, TrunkIoParams b) {
                        return b.trunk_name < a.trunk_name;
                    });
                break;

            case 1:
                if (order)
                    std::sort(trunkIoParams.begin(), trunkIoParams.end(), [](TrunkIoParams a, TrunkIoParams b) {
                        return b.capacity < a.capacity;
                    });
                else
                    std::sort(trunkIoParams.begin(), trunkIoParams.end(), [](TrunkIoParams a, TrunkIoParams b) {
                        return b.capacity > a.capacity;
                    });
                break;

            case 2:
                if (order)
                    std::sort(trunkIoParams.begin(), trunkIoParams.end(), [](TrunkIoParams a, TrunkIoParams b) {
                        return b.num_of_incoming < a.num_of_incoming;
                    });
                else
                    std::sort(trunkIoParams.begin(), trunkIoParams.end(), [](TrunkIoParams a, TrunkIoParams b) {
                        return b.num_of_incoming > a.num_of_incoming;
                    });
                break;

            case 4:
                if (order)
                    std::sort(trunkIoParams.begin(), trunkIoParams.end(), [](TrunkIoParams a, TrunkIoParams b) {
                        return b.num_of_outgoing < a.num_of_outgoing;
                    });
                else
                    std::sort(trunkIoParams.begin(), trunkIoParams.end(), [](TrunkIoParams a, TrunkIoParams b) {
                        return b.num_of_outgoing > a.num_of_outgoing;
                    });
                break;

            case 6:
                if (order)
                    std::sort(trunkIoParams.begin(), trunkIoParams.end(), [](TrunkIoParams a, TrunkIoParams b) {
                        return (b.num_of_outgoing+b.num_of_incoming) < (a.num_of_outgoing+a.num_of_incoming);
                    });
                else
                    std::sort(trunkIoParams.begin(), trunkIoParams.end(), [](TrunkIoParams a, TrunkIoParams b) {
                        return (b.num_of_outgoing+b.num_of_incoming) > (a.num_of_outgoing+a.num_of_incoming);
                    });
                break;

            case 8:
                if (order)
                    std::sort(trunkIoParams.begin(), trunkIoParams.end(), [](TrunkIoParams a, TrunkIoParams b) {
                        return b.getLoad() < a.getLoad();
                    });
                else
                    std::sort(trunkIoParams.begin(), trunkIoParams.end(), [](TrunkIoParams a, TrunkIoParams b) {
                        return b.getLoad() > a.getLoad();
                    });
        }
    }

    TrunkIoParams at(int i) { return trunkIoParams.at(i); }

    void set (int i, TrunkIoParams params) {

        if (i<this->trunkIoParams.size())
            this->trunkIoParams[i] = params;
    }
};

class PageTrunks : public BasePage {

    Repository repository;

public:
    bool canHandle(std::string &path) {
        return path == "/trunks";
    }
    void render(std::stringstream &html, map<string, string> &parameters) {

        html <<   "<head>\n"
             <<   "    <title>Active trunks</title>\n"
//             <<   "<meta http-equiv=\"refresh\" content=\"5\" >"
             <<   "</head>\n";

        renderHeader(html);

        if (!ready())
            return;

        shared_ptr<CurrentCdrList> cdrList = repository.currentCalls->currentCdr.get();
        if (cdrList == nullptr) {
            return;
        }
        shared_ptr<vector<Call>> calls = repository.currentCalls->getCallsWaitingSaving();

        ActiveTrunks activeTrunks(repository);
        html << "<body>\n";
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

        string sort_column = parameters["sort"];
        int sort_col_num = 0;
        if (sort_column.size()>0)
            sort_col_num = std::stoi(sort_column);

        string order = parameters["order"];
        bool asc = order.compare("1");
        order = asc? string("1"):string("0");

        bool fullMode = calls->size() == cdrList->size() * 2;
        html << "<table width=100% border=0 cellspacing=0>\n";
        html << "<tr>";
        if (sort_col_num==0)
            html << "<th nowrap><a href='/trunks?sort=0&order="+order+"'>trunk_name</a></th>";
        else
            html << "<th nowrap><a href='/trunks?sort=0&order=1'>trunk_name</a></th>";

        if (sort_col_num==1)
            html << "<th nowrap><a href='/trunks?sort=1&order="+order+"'>capacity</a></th>";
        else
            html << "<th nowrap><a href='/trunks?sort=1&order=0'>capacity</a></th>";

        if (sort_col_num==2)
            html << "<th nowrap><a href='/trunks?sort=2&order="+order+"'>incoming count</a></th>";
        else
            html << "<th nowrap><a href='/trunks?sort=2&order=0'>incoming count</a></th>";

        if (fullMode)
            html << "<th nowrap>incoming cost</th>";

        if (sort_col_num==4)
            html << "<th nowrap><a href='/trunks?sort=4&order="+order+"'>outgoing count</a></th>";
        else
            html << "<th nowrap><a href='/trunks?sort=4&order=0'>outgoing count</a></th>";

        if (fullMode)
            html << "<th nowrap>outgoing cost</th>";

        if (sort_col_num==6)
            html << "<th nowrap><a href='/trunks?sort=6&order="+order+"'>total count</a></th>";
        else
            html << "<th nowrap><a href='/trunks?sort=6&order=0'>total count</a></th>";

        if (fullMode)
            html << "<th nowrap>total cost</th>";

        if (sort_col_num==8)
            html << "<th nowrap><a href='/trunks?sort=8&order="+order+"'>load</a></th>";
        else
            html << "<th nowrap><a href='/trunks?sort=8&order=0'>load</a></th>";

        html << "</tr>";

        activeTrunks.sort(sort_col_num, asc);

        for (size_t i = 0; i < activeTrunks.count(); i++) {

            TrunkIoParams trunkParam = activeTrunks.at(i);
            html << "<tr class='tr_orig'>\n";
            html << "<td nowrap><a class=orig href='/calls?trunkname=" << trunkParam.trunk_name << "' style=\"text-decoration: none\">" << trunkParam.trunk_name << "</a></td>\n";

            html << "<td nowrap>" << trunkParam.capacity << "</td>\n";

            if (trunkParam.num_of_incoming>0)
                html << "<td nowrap><a class=orig href='/calls?trunkname=" << trunkParam.trunk_name << "&dest=in' style=\"text-decoration: none\">" << trunkParam.num_of_incoming << "</a></td>\n";
            else
                html << "<td nowrap>" << trunkParam.num_of_incoming << "</td>\n";

            if (fullMode)
                html << "<td nowrap>" << trunkParam.incoming_cost << "</td>\n";

            if (trunkParam.num_of_outgoing>0)
                html << "<td nowrap><a class=term href='/calls?trunkname=" << trunkParam.trunk_name << "&dest=out' style=\"text-decoration: none\">" << trunkParam.num_of_outgoing << "</a></td>\n";
            else
                html << "<td nowrap>" << trunkParam.num_of_outgoing << "</td>\n";

            if (fullMode)
                html << "<td nowrap>" << trunkParam.outgoing_cost << "</td>\n";

            html << "<td nowrap><a class=orig href='/calls?trunkname=" << trunkParam.trunk_name << "' style=\"text-decoration: none\">" << trunkParam.num_of_incoming+trunkParam.num_of_outgoing << "</a></td>\n";
            if (fullMode)
                html << "<td nowrap>" << trunkParam.incoming_cost+trunkParam.outgoing_cost << "</td>\n";

            html << "<td nowrap>" <<  trunkParam.getLoad() << "%</td>\n";

            html << "</tr>\n";
        }
        html << "</table>\n";
        html << "</body>\n";
    }

    bool ready() {

        if (!repository.billingData->ready())
            return false;

        return repository.prepare();
    }
};