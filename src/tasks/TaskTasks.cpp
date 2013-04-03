#include "TaskTasks.h"

TaskTasks::TaskTasks() {
    id = "tasks";
    name = "Tasks";
}

void TaskTasks::run()
{
    BDb db_main(app.conf.db_main);


    while(true){
        t.start();
        try{
            BDbResult res = db_main.query("select id, task, params from billing.tasks where region_id="+app.conf.str_region_id+" order by created desc limit 1");
            if(res.next())
            {
                string task_id = res.get(0);
                string task_name = res.get(1);
                string task_params = res.get(2);

                if (task_name == "recalc_this_month")
                {
                }
                else
                if (task_name == "recalc_last_month")
                {
                }
                else
                {
                    db_main.query("delete from billing.tasks where region_id="+app.conf.str_region_id+" and id="+task_id);
                }
            }
        }catch( DbException &e ){
            Log::er(e.what());
        }
        t.stop();

        ssleep(1);
    }
}


void TaskTasks::htmlfull(stringstream &html){
    this->html(html);

    html << "Time loop: <b>" << t.sloop() + "</b><br/>\n";
    html << "Time full loop: <b>" << t.sfull() + "</b><br/>\n";
    html << "loops: <b>" << t.count << "</b><br/>\n";
    html << "<br/>\n";
}
