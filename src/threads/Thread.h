#pragma once

#include "../common.h"
#include "../classes/Exception.h"
#include "../classes/Timer.h"
#include "../classes/Log.h"
#include "../classes/App.h"
#include <boost/thread.hpp>
#include <boost/signals2.hpp>

class Thread {
public:
    string id;
    string name;
    string status;

    boost::thread task_thread;
    boost::signals2::signal<void(Thread *) > onStarted;
    boost::signals2::signal<void(Thread *) > onFinished;

    Thread();
    virtual ~Thread();

    void start();

    static void ssleep(unsigned int seconds);

    static void usleep(unsigned int milliseconds);

    virtual bool ready();

    virtual void prepare();

    virtual void run() = 0;

    void operator()();

    void html(stringstream &html) {
        html << "<h3><a href='/task?id=" << this->id << "'>" << this->name << "</a> " << status << "</h3>\n";
    }

    virtual void htmlfull(stringstream &html) {
        this->html(html);
    }
};

typedef shared_ptr<Thread> pThread;