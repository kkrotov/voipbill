#pragma once

#include "../tasks/Task.h"
#include "../lists/CurrentCallsObjList.h"

class TaskCurrentCalls : public Task
{
public:
    static shared_ptr<CurrentCallsObjList> getList();
private:
    static shared_ptr<CurrentCallsObjList> list;
    static boost::detail::spinlock lock;

    Timer t;


    void run();


    void htmlfull(stringstream &html);

public:
    TaskCurrentCalls();
};

