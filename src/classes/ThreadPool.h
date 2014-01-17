#pragma once

#include <list>
#include <mutex>
using namespace std;

#include "../threads/Thread.h"

class ThreadPool {
public:
    std::mutex mutex;
    list<Thread *> threads;

    void run(Thread * thread);
    void register_thread(Thread * thread);
    void unregister_thread(Thread * thread);
    void thread_status_changed(Thread * thread);
    void thread_real_status_changed(Thread * thread);
    void app_status_changed();
    void app_real_status_changed();
    ThreadStatus getThreadStatusByAppStatus();

protected:
};
