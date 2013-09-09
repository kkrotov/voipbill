#include "ThreadCurrentCalls.h"
#include "../classes/DataLoader.h"
#include "../classes/CalcFull.h"

shared_ptr<CurrentCallsObjList> ThreadCurrentCalls::list(new CurrentCallsObjList());
boost::detail::spinlock ThreadCurrentCalls::lock;

shared_ptr<CurrentCallsObjList> ThreadCurrentCalls::getList() {
    lock.lock();
    shared_ptr<CurrentCallsObjList> plist = list;
    lock.unlock();
    return plist;
}

void ThreadCurrentCalls::run() {

    shared_ptr<CurrentCallsObjList> l(new CurrentCallsObjList());

    l->load(&db_rad, 0);

    lock.lock();
    ThreadCurrentCalls::list = l;
    lock.unlock();

}

void ThreadCurrentCalls::htmlfull(stringstream &html) {

    this->html(html);

    shared_ptr<CurrentCallsObjList> l = ThreadCurrentCalls::getList();

    CalcFull calculator;
    calculator.calc_limit(l.get());

    html << "Time loop: <b>" << this->t.sloop() << "</b><br/>\n";
    html << "Time full loop: <b>" << this->t.sfull() << "</b><br/>\n";
    html << "loops: <b>" << t.count << "</b><br/>\n";
    html << "<br/>\n";
    html << "Current calls: <b>" << l->count << "</b><br/>\n";
    html << "<br/>\n";

    html << "<table width=100% border=1 cellspacing=0>\n";
    html << "<tr>";
    html << "<th>time</th>";
    html << "<th>client</th>";
    html << "<th>usage_num</th>";
    html << "<th>direction</th>";
    html << "<th>phone_num</th>";
    html << "<th>len</th>";
    html << "<th>amount</th>";
    html << "<th>price</th>";
    html << "<th>pricelist</th>";
    html << "<th>operator</th>";
    html << "<th>amount_op</th>";
    html << "<th>price_op</th>";
    html << "<th>geo_id</th>";
    html << "</tr>\n";
    for (int i = 0; i < l->count; i++) {
        pCallObj call = l->get(i);

        html << "<tr>";
        html << "<td>" << call->time << "</td>";
        html << "<td>" << "<a href='/client?id=" << call->client_id << "'>" << (call->client_id ? lexical_cast<string>(call->client_id) : "") << "</a>" << "</td>";
        html << "<td>" << call->usage << "</td>";
        html << "<td><b>" << (call->out ? "out" : "in") << "</b></td>";
        html << "<td>" << call->phone << "</td>";
        html << "<td><b>" << call->len << "</b></td>";
        html << "<td><b>" << (call->pricelist_id != 0 || call->amount != 0 ? (call->freemin_group_id == 0 ? string_fmt("%.2f", call->amount / 100.0) : "free") : "&nbsp;") << "</b></td>";
        html << "<td>" << (call->pricelist_id != 0 || call->price != 0 ? string_fmt("%.2f", call->price / 10000.0) : "&nbsp;") << "</td>";
        html << "<td>" << (call->pricelist_id != 0 ? lexical_cast<string>(call->pricelist_id) : "&nbsp;") << "</td>";
        html << "<td>" << call->operator_id << (call->pricelist_op_id != 0 ? " / " + lexical_cast<string>(call->pricelist_op_id) : "&nbsp;") << "</td>";
        html << "<td>" << (call->pricelist_op_id != 0 || call->amount_op != 0 ? string_fmt("%.2f", call->amount_op / 100.0) : "&nbsp;") << "</td>";
        html << "<td>" << (call->pricelist_op_id != 0 || call->price_op != 0 ? string_fmt("%.2f", call->price_op / 10000.0) : "&nbsp;") << "</td>";
        html << "<td>" << (call->geo_id != 0 ? lexical_cast<string>(call->geo_id) : "&nbsp;") << "</td>";
        html << "</tr>\n";
    }
    html << "</table>\n";
}

ThreadCurrentCalls::ThreadCurrentCalls() {
    id = "currentcalls";
    name = "Current Calls";
    db_rad.setCS(app.conf.db_rad);
}


