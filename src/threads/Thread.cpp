#include "../common.h"
#include "Thread.h"


Thread::Thread(){
    id = string_fmt("%d", rand());
    name = id;
    status = "created";
}


Thread::~Thread(){
    app.unregister_thread(this);
}

void Thread::start(){
    std::thread t(&Thread::operator(), this);
    std::swap(task_thread, t);
}

void Thread::ssleep(unsigned int seconds)
{
    sleep(seconds);
}

void Thread::usleep(unsigned int milliseconds)
{
    struct timespec req;

    if (milliseconds >= 1000)
    {
        req.tv_sec = milliseconds / 1000;
        req.tv_nsec = (milliseconds % 1000) * 1000000L;
    }else{
        req.tv_sec = 0;
        req.tv_nsec = milliseconds;
    }

    clock_nanosleep(CLOCK_MONOTONIC, 0, &req, NULL);
}

void Thread::wait()
{
}

void Thread::prepare()
{
}

void Thread::operator()()
{

    app.register_thread(this);

    try
    {
        status = "waiting";
        this->wait();

        status = "preparing";
        this->prepare();

        status = "running";
        this->run();
    }
    catch( std::exception &e ){
        Log::er(e.what());
    }
    catch( ... ){
        Log::er("ERROR");
    }
    app.unregister_thread(this);

    delete this;
}
