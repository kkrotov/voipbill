#include "ThreadBillRuntime.h"
#include "../classes/AppBill.h"
#include "../../src/lists/RuntimeCallsObjList.h"

bool ThreadBillRuntime::need_refresh_current_id = false;

ThreadBillRuntime::ThreadBillRuntime() {
    id = "runtime";
    name = "Bill Runtime";

    calc_calls_full = 0;
    calc_calls_loop = 0;


    db_rad.setCS(app().conf.db_rad);
    db_calls.setCS(app().conf.db_calls);
    db_calls.needAdvisoryLock(app().conf.instance_id);
    calls_saver.setDb(&db_calls);
    calculator.setDb(&db_calls);
}

bool ThreadBillRuntime::ready() {
    return appBill().init_sync_done &&
            appBill().init_load_data_done &&
            appBill().init_load_counters_done;
}

void ThreadBillRuntime::run() {

    lock_guard<mutex> lk(app().bill_runnning_mutex);

repeat:

    if (need_refresh_current_id) {
        calls_list.refresh_current_id();
        need_refresh_current_id = false;
    }

    TimerScope ts1(t);

    if (calls_list.loaddata(&db_rad)) {

        boost::this_thread::interruption_point();

        {
            TimerScope ts2(t_calc);

            calculator.calc(&calls_list);
        }

        boost::this_thread::interruption_point();

        {
            TimerScope ts3(t_save);

            calls_saver.save(&calls_list);
            calculator.save();
        }

        boost::this_thread::interruption_point();

        calc_calls_loop = calls_list.count;
        calc_calls_full = calc_calls_full + calls_list.count;

        calls_list.next();

        if (calls_list.count >= calls_list.nrows)
            goto repeat;

    } else {
        calc_calls_loop = 0;
    }

    appBill().init_bill_runtime_started = true;
}

void ThreadBillRuntime::htmlfull(stringstream &html) {
    this->html(html);

    html << "radacctid><b>" << calls_list.last_id << "</b> and disconnectcause != <b>" << app().conf.billing_dc_break << "</b><br/>\n";
    html << "<br/>\n";

    html << "Time calc: <b>" << t_calc.sloop() << "</b><br/>\n";
    html << "Time save: <b>" << t_save.sloop() << "</b><br/>\n";
    html << "Time loop: <b>" << t.sloop() << "</b><br/>\n";
    html << "Time full calc: <b>" << t_calc.sfull() << "</b><br/>\n";
    html << "Time full save: <b>" << t_save.sfull() << "</b><br/>\n";
    html << "Time full loop: <b>" << t.sfull() << "</b><br/>\n";
    html << "loops: <b>" << t.count << "</b><br/>\n";
    html << "<br/>\n";

    html << "Count loop: <b>" << calc_calls_loop << "</b><br/>\n";
    html << "Count full: <b>" << calc_calls_full << "</b><br/>\n";
    calculator.html(html);
}
