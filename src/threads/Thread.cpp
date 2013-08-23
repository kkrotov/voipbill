#include "../common.h"
#include "Thread.h"

#include <chrono>

Thread::Thread() {
    id = string_fmt("%d", rand());
    name = id;
    status = "created";
}

Thread::~Thread() {
}

void Thread::start() {
    std::thread t(&Thread::operator(), this);
    std::swap(task_thread, t);
}

void Thread::ssleep(unsigned int seconds) {
    std::this_thread::sleep_for(std::chrono::seconds(seconds));
}

void Thread::usleep(unsigned int milliseconds) {
    std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
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
