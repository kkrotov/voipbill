#pragma once

#include <list>
#include <mutex>
using namespace std;

class Thread;

class ThreadPool {
public:
    std::mutex mutex;
    list<Thread *> threads;

    void run(Thread * thread);
    void register_thread(Thread * thread);
    void unregister_thread(Thread * thread);
protected:
};
