#pragma once

#include <list>
#include <mutex>
using namespace std;

#include "../threads/Thread.h"
class App;

class ThreadPool {
public:
    ThreadPool();
    
    void setApp(App * app);
    void run(Thread * thread);
    
    // Прервать выполнение всех потоков. Данный метод только отдаёт "команду",
    // не дожидаясь актуального завершения потоков.
    void shutdown();
    
    void app_status_changed();
    void app_real_status_changed();
    
    bool forAllThreads(std::function<bool(Thread*)> callback);
    
    boost::signals2::signal<void() > onLastThreadExits;
    
private:
    std::mutex mutex;
    list<Thread *> threads;
    App * app;
    
    void register_thread(Thread * thread);
    void unregister_thread(Thread * thread);
    void thread_status_changed(Thread * thread);
    void thread_real_status_changed(Thread * thread);
    ThreadStatus getThreadStatusByAppStatus();

    bool shutdownFlag;
};
