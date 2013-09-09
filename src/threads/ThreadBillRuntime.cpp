#include "ThreadBillRuntime.h"

#include "../lists/RuntimeCallsObjList.h"
#include "../classes/CallsSaver.h"

bool ThreadBillRuntime::need_refresh_current_id = false;

ThreadBillRuntime::ThreadBillRuntime() {
    id = "runtime";
    name = "Bill Runtime";

    calc_calls_full = 0;
    calc_calls_loop = 0;
}

bool ThreadBillRuntime::ready() {
    return app.init_sync_done &&
            app.init_load_data_done &&
            app.init_load_counters_done;
}

void ThreadBillRuntime::run() {
    Log::info("Running...");

    db_rad.setCS(app.conf.db_rad);

    db_calls.setCS(app.conf.db_calls);
    db_calls.needAdvisoryLock(app.conf.region_id);

    CallsSaver sv(&db_calls);
    calculator.setDb(&db_calls);

    while (true) {

        {

            lock_guard<mutex> lk(app.bill_runnning_mutex);

            if (need_refresh_current_id) {
                calls_list.refresh_current_id();
                need_refresh_current_id = false;
            }

            t.start();

            try {

                if (calls_list.loaddata(&db_rad)) {

                    t_calc.start();

                    calculator.calc(&calls_list);

                    t_calc.stop();

                    t_save.start();

                    sv.save(&calls_list);

                    calculator.save();

                    t_save.stop();


                    calc_calls_loop = calls_list.count;
                    calc_calls_full = calc_calls_full + calls_list.count;

                    calls_list.next();

                    if (calls_list.count >= calls_list.nrows) continue;
                } else {
                    calc_calls_loop = 0;
                }

                app.init_bill_runtime_started = true;

            } catch (Exception &e) {
                e.addTrace("ThreadBillRuntime::run");
                Log::exception(e);
            }

            t.stop();

        }

        ssleep(1);
    }

}

void ThreadBillRuntime::htmlfull(stringstream &html) {
    this->html(html);

    html << "radacctid><b>" << calls_list.last_id << "</b> and disconnectcause != <b>" << app.conf.billing_dc_break << "</b><br/>\n";
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
