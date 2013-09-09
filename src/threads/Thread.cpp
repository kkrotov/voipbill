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

void Thread::wait() {
}

void Thread::prepare() {
}

void Thread::operator()() {

    onStarted(this);

    try {
        status = "waiting";
        this->wait();

        status = "preparing";
        this->prepare();

        status = "running";
        this->run();
    } catch (std::exception &e) {
        Log::error("Thread::operator: " + string(e.what()));
    } catch (...) {
        Log::error("Thread::ERROR");
    }

    onFinished(this);
}
