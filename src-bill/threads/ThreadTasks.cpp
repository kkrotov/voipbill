#include "ThreadTasks.h"
#include "../classes/AppBill.h"
#include "../tasks/TaskRecalc.h"
#include "../data/DataContainer.h"
#include "../data/DataBillingContainer.h"

ThreadTasks::ThreadTasks() {
    id = idName();
    name = "Tasks";
    tasks_count = 0;
    db_main.setCS(app().conf.db_main);
    threadSleepSeconds = 5;
}

bool ThreadTasks::ready() {

    PreparedData preparedData;
    if (!DataContainer::instance()->prepareData(preparedData, time(nullptr))) {
        return false;
    }

    if (!DataBillingContainer::instance()->ready()) {
        return false;
    }

    return true;
}

void ThreadTasks::run() {

    current_task.reset();

    BDbResult res = db_main.query("select id, task, params from billing.tasks where region_id=" + app().conf.str_instance_id + " order by created desc limit 1");
    if (res.next()) {
        string task_id = res.get(0);
        string task_name = res.get(1);
        string task_params = res.get(2);

        if (task_name == "recalc_current_month") {
            time_t rawtime = time(NULL);
            struct tm timeinfo;
            gmtime_r(&rawtime, &timeinfo);
            timeinfo.tm_mday = 1;
            timeinfo.tm_isdst = 0;
            timeinfo.tm_wday = 0;
            timeinfo.tm_yday = 0;
            timeinfo.tm_hour = 0;
            timeinfo.tm_min = 0;
            timeinfo.tm_sec = 0;

            time_t t_current_month = timegm(&timeinfo);
            t_current_month -= 12 * 3600;

            TaskRecalc *task = new TaskRecalc(t_current_month, app().conf.str_instance_id);
            task->initTask(db_main, task_id, task_params);
            current_task.reset(task);
            task->run();

            db_main.exec(
                    "delete from billing.tasks where region_id=" + app().conf.str_instance_id + " and id=" + task_id);
        } else {
            if (task_name == "recalc_last_month") {
                time_t rawtime = time(NULL);
                struct tm timeinfo;
                gmtime_r(&rawtime, &timeinfo);
                timeinfo.tm_mday = 1;
                timeinfo.tm_isdst = 0;
                timeinfo.tm_wday = 0;
                timeinfo.tm_yday = 0;
                timeinfo.tm_hour = 0;
                timeinfo.tm_min = 0;
                timeinfo.tm_sec = 0;

                if (--timeinfo.tm_mon < 0) {
                    timeinfo.tm_mon = 11;
                    timeinfo.tm_year--;
                }

                time_t t_previous_month = timegm(&timeinfo);
                t_previous_month -= 12 * 3600;

                TaskRecalc *task = new TaskRecalc(t_previous_month, app().conf.str_instance_id);
                task->initTask(db_main, task_id, task_params);
                current_task.reset(task);
                task->run();

                db_main.exec("delete from billing.tasks where region_id=" + app().conf.str_instance_id + " and id=" + task_id);
            }
        }

        tasks_count++;

        current_task.reset();


    }
}

void ThreadTasks::htmlfull(stringstream &html) {
    this->html(html);

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

bool ThreadTasks::hasFullHtml() {
    return true;
}
