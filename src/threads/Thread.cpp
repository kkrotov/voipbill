#include "../common.h"
#include "Thread.h"

Thread::Thread() {
    id = string_fmt("%d", rand());
    name = id;
    status = "created";
}

Thread::~Thread() {
}

void Thread::start() {
    boost::thread t(&Thread::operator(), this);
    std::swap(task_thread, t);
}

void Thread::ssleep(unsigned int seconds) {
    boost::this_thread::sleep_for(boost::chrono::seconds(seconds));
}

void Thread::usleep(unsigned int milliseconds) {
    boost::this_thread::sleep_for(boost::chrono::milliseconds(milliseconds));
}

bool Thread::ready() {
    return true;
}

bool Thread::prepare() {
    return true;
}

void Thread::operator()() {

    onStarted(this);

    try {
        status = "waiting";
        while (!this->ready()) {
            ssleep(1);
        }

        status = "preparing";
        while (!this->prepare()) {
            ssleep(1);
        }
    } catch (std::exception &e) {
        Log::error("Thread::operator: " + string(e.what()));
    } catch (...) {
        Log::error("Thread::ERROR");
    }

    while (true) {
        try {
            status = "running";
            TimerScope ts(t);
            this->run();
        } catch (Exception &e) {
            e.addTrace("Thread(" + name + "): ");
            Log::exception(e);
        } catch (std::exception &e) {
            Log::error("Thread(" + name + "): " + string(e.what()));
        } catch (...) {
            Log::error("Thread(" + name + "): ERROR");
        }
        ssleep(1);
    }

    onFinished(this);
}
