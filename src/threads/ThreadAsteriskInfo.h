#pragma once

#include "../threads/Thread.h"
#include "../lists/AsteriskNumberObjList.h"
#include <boost/smart_ptr/detail/spinlock.hpp>

class ThreadAsteriskInfo : public Thread {
    /*
    public:
        static shared_ptr<AsteriskNumberObjList> getList();
    private:
        static shared_ptr<AsteriskNumberObjList> list;
        static boost::detail::spinlock lock;

        Timer t;

        void run();

        void htmlfull(stringstream &html);
    public:
        ThreadAsteriskInfo();
     */
};

