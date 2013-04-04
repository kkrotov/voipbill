#include "TaskRecalc.h"

#include "../common.h"
#include "../classes/App.h"
#include "../classes/CallsSaver.h"
#include "../threads/ThreadLoader.h"
#include "../threads/ThreadBillRuntime.h"


TaskRecalc::TaskRecalc(time_t date_from)
{
    this->date_from = date_from;
    calc_calls_loop = 0;
    calc_calls_full = 0;
    name = "Recalc from "+string_time(date_from);
}

void TaskRecalc::run()
{
    lock_guard<mutex> lk(app.bill_runnning_mutex);

    setStatus("1. getting min id");

    BDb db_rad(app.conf.db_rad);
    BDb db_calls(app.conf.db_calls);
    BDbResult res = db_calls.query("select min(id) from billing.calls where time>='"+string_time(date_from)+"'");

    long long int current_call_id = 0;
    if (res.next())
    {
        current_call_id = res.get_ll(0);
    }

    setStatus("2. delete calls from id "+lexical_cast<string>(current_call_id));
    db_calls.exec("delete from billing.calls where id>="+lexical_cast<string>(current_call_id));

    CallsSaver sv(&db_calls);

    DataLoader data_loader;
    ThreadLoader t_loader;

    setStatus("3. recalc temp data");
    if (!t_loader.do_load_data(&db_calls, &data_loader))
    {
        throw DbException("Can not load temp data");
    }
    setStatus("4. recalc temp counters");
    if (!t_loader.do_load_counters(&db_calls, &data_loader))
    {
        throw DbException("Can not load temp counters");
    }

    calculator.setDb(&db_calls);
    calculator.setDataLoader(&data_loader);

    setStatus("5. calc");

    while(true){


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
        }

        break;
    }

    setStatus("6. recalc counters");

    if (!t_loader.do_load_counters(&db_calls))
    {
        throw DbException("Can not load counters");
    }

    ThreadBillRuntime::need_refresh_current_id = true;

    setStatus("7. delete calls from main");
    db_main->exec("delete from billing.calls_"+app.conf.str_region_id+" where id>="+lexical_cast<string>(current_call_id));

}

void TaskRecalc::html(stringstream &html)
{
    html << "Count loop: <b>" << calc_calls_loop << "</b><br/>\n";
    html << "Count full: <b>" << calc_calls_full << "</b><br/>\n";
    calculator.html(html);
}
