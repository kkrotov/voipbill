#pragma once

#include "../common.h"
#include "../classes/Exception.h"
#include "../classes/Timer.h"
#include "../classes/Log.h"
#include <boost/thread.hpp>
#include <boost/signals2.hpp>

enum ThreadStatus {
    THREAD_CREATED, THREAD_WAITING, THREAD_PREPARING, THREAD_RUNNING, THREAD_PAUSED, THREAD_STOPPED
};

static const char * ThreadStatusNames[] = {
    "CREATED", "WAITING", "PREPARING", "RUNNING", "PAUSED", "STOPPED"
};

class Thread {
public:
    string id;
    string name;
    ThreadStatus status;
    ThreadStatus real_status;

    bool status_ready;
    bool status_prepared;

    boost::thread task_thread;
    boost::signals2::signal<void(Thread *) > onStarted;
    boost::signals2::signal<void(Thread *) > onFinished;
    boost::signals2::signal<void(Thread *) > onStatusChanged;
    boost::signals2::signal<void(Thread *) > onRealStatusChanged;

    int threadSleepSeconds;

    Thread();
    virtual ~Thread();

    void start();

    static void ssleep(unsigned int seconds);

    static void usleep(unsigned int milliseconds);

    virtual bool ready();

    virtual bool prepare();

    virtual void run() = 0;

    void setStatus(ThreadStatus status);
    void setRealStatus(ThreadStatus real_status);
    ThreadStatus getStatus();
    ThreadStatus getRealStatus();

    void operator()();

    void html(stringstream &html) {
        ThreadStatus status = getStatus();
        ThreadStatus real_status = getRealStatus();

        html << "<div>";
        html << "<a href='/task?id=" << this->id << "'>" << this->name << "</a> ";
        html << "<b>" << ThreadStatusNames[real_status] << "</b>";
        if (status != real_status) {
            html << " (" << ThreadStatusNames[status] << ")";
        }
        html << "</div>\n";
    }

    virtual void htmlfull(stringstream &html) {
        this->html(html);
    }
protected:
    Timer t;
};

typedef shared_ptr<Thread> pThread;