#include "ThreadTasks.h"
#include "../classes/AppAuth.h"

ThreadTasks::ThreadTasks() {
    id = "tasks";
    name = "Tasks";
    tasks_count = 0;
    db_main.setCS(app().conf.db_main);
}

bool ThreadTasks::ready() {
    return appAuth().init_sync_done &&
            appAuth().init_load_data_done &&
            appAuth().init_load_counters_done &&
            appAuth().init_bill_runtime_started;
}

void ThreadTasks::run() {

    current_task.reset();

    BDbResult res = db_main.query("select id, task, params from billing.tasks where region_id=" + app().conf.str_instance_id + " order by created desc limit 1");
    if (res.next()) {
        string task_id = res.get(0);
        string task_name = res.get(1);
        string task_params = res.get(2);

        /*
         tasks handling
         */

        tasks_count++;

        current_task.reset();

        boost::this_thread::interruption_point();
    }
}

void ThreadTasks::htmlfull(stringstream &html) {
    this->html(html);

    html << "Time loop: <b>" << t.sloop() + "</b><br/>\n";
    html << "Time full loop: <b>" << t.sfull() + "</b><br/>\n";
    html << "Loops: <b>" << t.count << "</b><br/>\n";
    html << "<br/>\n";
    html << "Tasks count: <b>" << lexical_cast<string>(tasks_count) << "</b><br/>\n";
    html << "<br/>\n";

    if (current_task != 0) {
        html << "Task: <b>" + current_task->getName() + "</b><br/>\n";
        html << "Status: <b>" + current_task->getStatus() + "</b><br/>\n";
        html << "<br/>\n";
        current_task->html(html);
        html << "<br/>\n";
    }
}
