#include "ThreadPool.h"
#include "../threads/Thread.h"

void ThreadPool::run(Thread * thread) {
    thread->onStarted.connect(boost::bind(&ThreadPool::register_thread, this, _1));
    thread->onFinished.connect(boost::bind(&ThreadPool::unregister_thread, this, _1));
    thread->start();
}

void ThreadPool::register_thread(Thread *thread) {
    lock_guard<std::mutex> lock(mutex);

    auto it = threads.begin();
    while (it != threads.end()) {
        Thread * tmp = *it;
        if (tmp->id == thread->id) {
            if (tmp == thread) {
                return;
            }
            thread->id = thread->id + string_fmt("%d", rand());
            break;
        }
        ++it;
    }
    threads.push_back(thread);
}

void ThreadPool::unregister_thread(Thread *thread) {
    lock_guard<std::mutex> lock(mutex);

    auto it = threads.begin();
    while (it != threads.end()) {
        if (*it == thread) {
            thread->task_thread.detach();
            threads.erase(it);
            delete thread;
            break;
        }
        ++it;
    }
}
