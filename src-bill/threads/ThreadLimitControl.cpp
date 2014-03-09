#include "ThreadLimitControl.h"
#include "../classes/AppBill.h"
#include "../../src/classes/CallsSaver.h"
#include "../../src/classes/KillCalls.h"
#include "../../src/lists/CurrentCallsObjList.h"
#include "ThreadCurrentCalls.h"

ThreadLimitControl::ThreadLimitControl() {
    id = "limitcontrol";
    name = "Limit control";
    db_calls.setCS(app().conf.db_calls);
    calculator.setDb(&db_calls);
}

bool ThreadLimitControl::ready() {
    return appBill().init_sync_done &&
            appBill().init_load_data_done &&
            appBill().init_load_counters_done &&
            appBill().init_bill_runtime_started;
}

void ThreadLimitControl::run() {

    shared_ptr<CurrentCallsObjList> splist = ThreadCurrentCalls::getList();
    CurrentCallsObjList * list = splist.get();

    if (list->loadtime + 60 >= time(NULL)) {

        {
            TimerScope ts2(t_calc);

            calculator.calc_limit(list);
        }

        {
            TimerScope ts3(t_kill);

            KillCalls::kill(list);
        }
    }

}

void ThreadLimitControl::htmlfull(stringstream &html) {
    this->html(html);

    html << "Time calc: <b>" << t_calc.sloop() + "</b><br/>\n";
    html << "Time kill: <b>" << t_kill.sloop() + "</b><br/>\n";
    html << "Time loop: <b>" << t.sloop() + "</b><br/>\n";
    html << "Time full calc: <b>" << t_calc.sfull() + "</b><br/>\n";
    html << "Time full kill: <b>" << t_kill.sfull() + "</b><br/>\n";
    html << "Time full loop: <b>" << t.sfull() + "</b><br/>\n";
    html << "loops: <b>" << t.count << "</b><br/>\n";
    html << "<br/>\n";
    calculator.html(html);
}

