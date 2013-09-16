#include "../common.h"
#include "Thread.h"

Thread::Thread() {
    id = string_fmt("%d", rand());
    name = id;

    status = ThreadStatus::THREAD_CREATED;
    setRealStatus(ThreadStatus::THREAD_CREATED);

    status_ready = false;
    status_prepared = false;
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

    while (true) {

        try {
            if (getStatus() == ThreadStatus::THREAD_STOPPED) {
                setRealStatus(ThreadStatus::THREAD_STOPPED);
                onFinished(this);
                return;
            } else if (getStatus() == ThreadStatus::THREAD_PAUSED) {
                setRealStatus(ThreadStatus::THREAD_PAUSED);
            } else if (getStatus() == ThreadStatus::THREAD_RUNNING || getStatus() == ThreadStatus::THREAD_PREPARING) {
                if (!status_ready) {
                    setRealStatus(ThreadStatus::THREAD_WAITING);
                    if (this->ready()) {
                        status_ready = true;
                        continue;
                    }
                } else if (!status_prepared) {
                    setRealStatus(ThreadStatus::THREAD_PREPARING);
                    if (this->prepare()) {
                        status_prepared = true;
                        continue;
                    }
                } else {
                    if (getStatus() == ThreadStatus::THREAD_RUNNING) {
                        setRealStatus(ThreadStatus::THREAD_RUNNING);
                        TimerScope ts(t);
                        this->run();
                    } else {
                        setRealStatus(ThreadStatus::THREAD_PAUSED);
                    }
                }
            }


            ssleep(1);
            continue;

        } catch (boost::thread_interrupted const& e) {
            continue;
        } catch (Exception &e) {
            e.addTrace("Thread(" + name + "): ");
            Log::exception(e);
        } catch (std::exception &e) {
            Log::error("Thread(" + name + "): " + string(e.what()));
        } catch (...) {
            Log::error("Thread(" + name + "): ERROR");
        }

        try {
            ssleep(1);
        } catch (boost::thread_interrupted const& e) {
            continue;
        }
    }

    onFinished(this);
}

void Thread::setStatus(ThreadStatus status) {
    this->status = status;
    onStatusChanged(this);
    task_thread.interrupt();
}

void Thread::setRealStatus(ThreadStatus real_status) {
    this->real_status = real_status;
    onRealStatusChanged(this);
}

ThreadStatus Thread::getStatus() {
    return status;
}

ThreadStatus Thread::getRealStatus() {
    return real_status;
}