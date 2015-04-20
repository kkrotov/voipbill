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
    // Инициализируется ThreadPool'ом - переделать: либо инициализировать через
    // аргумент в конструкторе, либо через setStatus().
    ThreadStatus status;

    boost::signals2::signal<void(Thread *) > onStarted;
    boost::signals2::signal<void(Thread *) > onFinished;
    boost::signals2::signal<void(Thread *) > onRealStatusChanged;

    boost::thread task_thread;

    virtual ~Thread();

    void start(int exitAfterRunsCount = 0, bool skipPrepare = false);

    ThreadStatus getStatus();
    void setStatus(ThreadStatus status);
    
    ThreadStatus getRealStatus();
    
    void threadTotals(stringstream &html) {
        ThreadStatus status = getStatus();
        ThreadStatus real_status = getRealStatus();

        html << "<tr>";
        html << "<td style='text-align: left'><a href='/task?id=" << this->id << "'>" << this->name << "</a></td>\n";
        html << "<td style='text-align: left'>" << ThreadStatusNames[real_status] << (status != real_status ? " (" + string(ThreadStatusNames[status]) + ")" : "") << "</b></td>\n";
        html << "<td style='text-align: left'>Count: <b>" << timer.count << "</b></td>\n";
        html << "<td style='text-align: left'>Time last: <b>" << timer.sloop() << "</b></td>\n";
        html << "<td style='text-align: left'>Time total: <b>" << timer.sloop() << "</b></td>\n";
        html << "<td style='text-align: left'>Errors: <b>" << errorsCount << "</b></td>\n";
        html << "<td style='text-align: left'>" << lastError << "</td>\n";
        html << "</tr>";
    }

    void html(stringstream &html) {
        html << "<table style='width:100%'>\n";
        this->threadTotals(html);
        html << "</table>\n";
        html << "<hr/><br/>\n";
    }
    
    virtual void htmlfull(stringstream &html) {
        this->html(html);
    }

protected:
    string name;
    
    int threadSleepSeconds;
    Timer timer;
    int errorsCount = 0;
    string lastError;
    
    Thread();

    static void ssleep(unsigned int seconds);
    
private:
    ThreadStatus real_status;

    bool status_ready;
    bool status_prepared;
    
    int exitAfterRunsCount;
    int runsCount;

    static void usleep(unsigned int milliseconds);

    virtual bool ready();

    virtual bool prepare();

    virtual void run() = 0;
    
    void setRealStatus(ThreadStatus real_status);
    virtual void onShutdown() { }

    void operator()();
};

typedef shared_ptr<Thread> pThread;