#include "TaskBillRuntime.h"

#include "../CallsSaver.h"

TaskBillRuntime::TaskBillRuntime() {
    id = "runtime";
    name = "Bill Runtime";
}

string TaskBillRuntime::sql(BDb * db){
    if (last_id == 0){
        BDbResult res = db_calls.query("select max(id) from billing.calls");
        if(res.next()){
            last_id = res.get_ll(0);
        }else{
            last_id = 100000000;
        }
    }
    return	"	select radacctid, connecttime, acctsessiontime, direction, " \
                    "       substring(callingstationid from '^\\d{1,18}')::bigint, " \
                    "       substring(calledstationid from '^\\d{1,18}')::bigint, " \
                    "       in_oper, out_oper, prefix_des between 21 and 24 " \
                    "	from radacct_voip_stop " \
                    "	where radacctid>'"+lexical_cast<string>(last_id)+"' " \
                    "       and direction in ('in','out') " \
                    "		and disconnectcause != "+lexical_cast<string>( app.conf.billing_dc_break ) +
                    "	order by radacctid " \
                    "	limit " + lexical_cast<string>(nrows);
}

void TaskBillRuntime::parse_item(BDbResult &row, void * obj){
    pCallObj item = (pCallObj)obj;
    strncpy((char*)item->id, row.get(0), 20);
    item->id_num = row.get_ll(0);
    strncpy((char*)item->time, row.get(1), 25);
    item->make_dt();
    item->len = row.get_i(2);
    item->out = (strcmp(row.get(3), "out") == 0);
    if (item->out){
        strcpy((char*)&item->usage, row.get(4));
        strcpy((char*)&item->phone, row.get(5));
        item->region = row.get_i(6);
        item->operator_id = row.get_i(7);
    }else{
        strcpy((char*)&item->phone, row.get(4));
        strcpy((char*)&item->usage, row.get(5));
        item->region = row.get_i(7);
        item->operator_id = row.get_i(6);
    }
    item->redirect = row.get_b(8);
    if (item->region == 0) item->region = app.conf.region_id;
    item->kill_call_reason = 0;
}

void TaskBillRuntime::wait()
{
    while(app.init_sync_done == false ||
          app.init_load_data_done == false ||
          app.init_load_counters_done == false)
    {
        ssleep(1);
    }
}

void TaskBillRuntime::run()
{
    db_rad.setCS(app.conf.db_rad);
    db_calls.setCS(app.conf.db_calls);

    calc_calls_full = 0;
    init(0, 0, 50000);

    CallsSaver sv(&db_calls);
    calculator.setDb(&db_calls);

    while(true){

        t.start();

        try{

            if (loaddata(&db_rad)) {

                t_calc.start();

                calculator.calc(this);

                t_calc.stop();

                t_save.start();

                sv.save(this);

                calculator.save();

                t_save.stop();


                calc_calls_loop = count;
                calc_calls_full = calc_calls_full + count;

                next();

                if (count >= 50000) continue;
            }else{
                calc_calls_loop = 0;
            }

            app.init_bill_runtime_started = true;

        }catch( DbException &e ){
            Log::er(e.what());
        }

        t.stop();

        ssleep(1);
    }

}


void TaskBillRuntime::htmlfull(stringstream &html){
    this->html(html);

    html << "radacctid><b>" << last_id << "</b> and disconnectcause != <b>" << app.conf.billing_dc_break << "</b><br/>\n";
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
