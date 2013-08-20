#include "../common.h"
#include "Thread.h"

#include <chrono>

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
    std::this_thread::sleep_for(std::chrono::seconds(seconds));
}

void Thread::usleep(unsigned int milliseconds)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
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
}
