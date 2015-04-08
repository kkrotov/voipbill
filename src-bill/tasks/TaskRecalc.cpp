#include "TaskRecalc.h"

#include "../../src/common.h"
#include "../classes/AppBill.h"
#include "../classes/CallsSaver.h"
#include "../threads/ThreadLoader.h"
#include "../threads/ThreadBillRuntime.h"

TaskRecalc::TaskRecalc(time_t date_from, string server_id) : Task(server_id) {
    this->date_from = date_from;
    calc_calls_loop = 0;
    calc_calls_full = 0;
    name = "Recalc from " + string_time(date_from);
}

void TaskRecalc::run() {
    lock_guard<mutex> lk(app().bill_runnning_mutex);

    setStatus("1. getting min id");

    BDb db_calls(app().conf.db_calls);
    BDbResult res = db_calls.query("select min(id) from calls.calls where time>='" + string_time(date_from) + "'");

    long long int current_call_id = 0;
    if (res.next()) {
        current_call_id = res.get_ll(0);
    }

    boost::this_thread::interruption_point();

    setStatus("2. delete calls from id " + lexical_cast<string>(current_call_id));
    db_calls.exec("delete from calls.calls where id>=" + lexical_cast<string>(current_call_id));

    CallsSaver sv(&db_calls);

    DataLoader data_loader;
    ThreadLoader t_loader;

    boost::this_thread::interruption_point();

    setStatus("3. recalc temp data");
    if (!t_loader.do_load_data(&db_calls, &data_loader)) {
        throw Exception("Can not load temp data");
    }

    boost::this_thread::interruption_point();

    setStatus("4. recalc temp counters");
    if (!t_loader.do_load_counters(&db_calls, &data_loader)) {
        throw Exception("Can not load temp counters");
    }

    calculator.setDb(&db_calls);
    calculator.setDataLoader(&data_loader);

    boost::this_thread::interruption_point();

    setStatus("5. calc");

    while (true) {


        if (cdr_list.loaddata(&db_calls)) {

            {
                TimerScope ts1(t_calc);

//                calculator.calc(&cdr_list);
            }

            {
                TimerScope ts2(t_save);

//                sv.save(&cdr_list);

//                calculator.save();
            }

            calc_calls_loop = cdr_list.count;
            calc_calls_full = calc_calls_full + cdr_list.count;

            cdr_list.next();

            boost::this_thread::interruption_point();

            setStatus("5. calc " + lexical_cast<string>(calc_calls_full) + " " + string_time(calculator.last_call_time));

            if (cdr_list.count >= cdr_list.nrows) continue;
        }

        break;
    }

    boost::this_thread::interruption_point();

    setStatus("6. recalc counters");

    if (!t_loader.do_load_counters(&db_calls)) {
        throw Exception("Can not load counters");
    }

    ThreadBillRuntime::need_refresh_current_id = true;

    boost::this_thread::interruption_point();

    setStatus("7. delete calls from main");
    db_main->exec("delete from calls.calls_" + app().conf.str_instance_id + " where id>=" + lexical_cast<string>(current_call_id));

}

void TaskRecalc::html(stringstream &html) {
    html << "Count loop: <b>" << calc_calls_loop << "</b><br/>\n";
    html << "Count full: <b>" << calc_calls_full << "</b><br/>\n";
    calculator.html(html);
}
