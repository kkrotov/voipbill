#include "Task.h"

#include "../classes/App.h"

void Task::initTask(BDb &db_main, string id, string params)
{
    this->db_main = &db_main;
    this->id = id;
    this->params = params;
}

void Task::setStatus(string str)
{
    status_lock.lock();
    status = str;
    status_lock.unlock();
    db_main->exec("update billing.tasks set status='"+str+"' where region_id="+app.conf.str_region_id+" and id="+id);
}

string Task::getStatus()
{
    status_lock.lock();
    string str = status;
    status_lock.unlock();
    return str;
}

string Task::getName()
{
    return name;
}

string Task::getParams()
{
    return name;
}

void Task::html(stringstream &html)
{
}
