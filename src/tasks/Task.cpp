#include "../common.h"
#include "Task.h"

#include <boost/thread.hpp>

Task::Task(){
    id = string_fmt("%d", rand());
    name = id;
    status = "created";
}

void Task::start(){
    boost::thread thread(&Task::operator(), this);
}

void Task::ssleep(long seconds)
{
    sleep(seconds);
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
