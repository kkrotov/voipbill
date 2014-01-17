#include "Task.h"

#include "../classes/App.h"

void Task::initTask(BDb &db_main, string id, string params) {
    this->db_main = &db_main;
    this->id = id;
    this->params = params;
}

void Task::setStatus(string str) {
    {
        lock_guard<spinlock> lock(status_lock);
        status = str;
    }
    db_main->exec("update billing.tasks set status='" + str + "' where region_id=" + app.conf.str_instance_id + " and id=" + id);
}

string Task::getStatus() {
    lock_guard<spinlock> lock(status_lock);
    string str = status;
    return str;
}

string Task::getName() {
    return name;
}

string Task::getParams() {
    return name;
}

void Task::html(stringstream &html) {
}
