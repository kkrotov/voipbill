#include "ThreadTasks.h"
#include "../classes/App.h"
#include "../tasks/TaskRecalc.h"

ThreadTasks::ThreadTasks() {
    id = "tasks";
    name = "Tasks";
    tasks_count = 0;
    db_main.setCS(app.conf.db_main);
}

bool ThreadTasks::ready() {
    return app.init_sync_done &&
            app.init_load_data_done &&
            app.init_load_counters_done &&
            app.init_bill_runtime_started;
}

void ThreadTasks::run() {

    current_task.reset();

    BDbResult res = db_main.query("select id, task, params from billing.tasks where region_id=" + app.conf.str_region_id + " order by created desc limit 1");
    if (res.next()) {
        string task_id = res.get(0);
        string task_name = res.get(1);
        string task_params = res.get(2);

        if (task_name == "recalc_current_month") {
            time_t rawtime = time(NULL);
            struct tm *ttt;
            ttt = localtime(&rawtime);
            ttt->tm_mday = 1;
            ttt->tm_isdst = 0;
            ttt->tm_wday = 0;
            ttt->tm_yday = 0;
            ttt->tm_hour = 0;
            ttt->tm_min = 0;
            ttt->tm_sec = 0;

            TaskRecalc *task = new TaskRecalc(mktime(ttt));
            task->initTask(db_main, task_id, task_params);
            current_task.reset(task);
            task->run();
        } else
            if (task_name == "recalc_last_month") {
            time_t rawtime = time(NULL);
            struct tm *ttt;
            ttt = localtime(&rawtime);
            ttt->tm_mday = 1;
            ttt->tm_isdst = 0;
            ttt->tm_wday = 0;
            ttt->tm_yday = 0;
            ttt->tm_hour = 0;
            ttt->tm_min = 0;
            ttt->tm_sec = 0;

            if (--ttt->tm_mon < 0) {
                ttt->tm_mon = 11;
                ttt->tm_year--;
            }

            TaskRecalc *task = new TaskRecalc(mktime(ttt));
            task->initTask(db_main, task_id, task_params);
            current_task.reset(task);
            task->run();
        }

        db_main.exec("delete from billing.tasks where region_id=" + app.conf.str_region_id + " and id=" + task_id);

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
