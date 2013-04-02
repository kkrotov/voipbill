#include "../common.h"
#include "Task.h"


Task::Task(){
    id = string_fmt("%d", rand());
    name = id;
    status = "created";
}


Task::~Task(){
    app.unregister_task(this);
}

void Task::start(){
    std::thread t(&Task::operator(), this);
    std::swap(task_thread, t);
}

void Task::ssleep(unsigned int seconds)
{
    sleep(seconds);
}

void Task::usleep(unsigned int milliseconds)
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

void Task::wait()
{
}

void Task::prepare()
{
}

void Task::operator()()
{

    app.register_task(this);

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
    app.unregister_task(this);

    delete this;
}
