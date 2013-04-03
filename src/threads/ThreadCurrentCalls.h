#pragma once

#include "../threads/Thread.h"
#include "../lists/CurrentCallsObjList.h"
#include <boost/smart_ptr/detail/spinlock.hpp>

class ThreadCurrentCalls : public Thread
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
    ThreadCurrentCalls();
};

