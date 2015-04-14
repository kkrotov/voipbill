#include "ThreadSelectCurrentCalls.h"
#include "../lists/CurrentCdrList.h"
#include "../classes/Billing.h"
// #include "../classes/CalcFull.h"


void ThreadSelectCurrentCalls::run() {

    data->currentCdrData.load(&db_calls);
}

void ThreadSelectCurrentCalls::htmlfull(stringstream &html) {

    this->html(html);

    if (!data->currentCdrData.ready()) {
        return;
    }

    shared_ptr<CurrentCdrList> cdrList = data->currentCdrData.get();

    Billing billing;
    billing.calcCurrentCdrs();

//    BDb db_main;
//    db_main.setCS(app().conf.db_main);
//    CalcFull calculator;
//    calculator.setDb(&db_main);
    //calculator.calc_current(cdrList.get());

    html << "Time loop: <b>" << this->t.sloop() << "</b><br/>\n";
    html << "Time full loop: <b>" << this->t.sfull() << "</b><br/>\n";
    html << "loops: <b>" << t.count << "</b><br/>\n";
    html << "<br/>\n";
    html << "Current calls: <b>" << cdrList->size() << "</b><br/>\n";
    html << "<br/>\n";

    html << "<table width=100% border=1 cellspacing=0>\n";
    html << "<tr>";
    html << "<th>connect_time</th>";
    html << "<th>src_number</th>";
    html << "<th>dst_number</th>";
    html << "<th>redirect_number</th>";
    html << "<th>src_route</th>";
    html << "<th>dst_route</th>";
    html << "</tr>\n";
    for (size_t i = 0; i < cdrList->size(); i++) {
        auto cdr = cdrList->get(i);

        html << "<tr>";
        html << "<td>" << cdr->connect_time << "</td>";
        html << "<td>" << cdr->src_number << "</td>";
        html << "<td>" << cdr->dst_number << "</td>";
        html << "<td>" << cdr->redirect_number << "</td>";
        html << "<td>" << cdr->src_route << "</td>";
        html << "<td>" << cdr->dst_route << "</td>";
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


