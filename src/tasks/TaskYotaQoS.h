#pragma once
#include "Task.h"
#include "../classes/Timer.h"

#include <map>

class TaskYotaQoS : public Task
{
protected:
    Timer t;
    Timer t_api;

    map<long long int, string> sessions;

    int sessions_count;
    int new_sessions_count;
    int old_sessions_count;
    int deleted_sessions_count;

public:

    TaskYotaQoS();

    void run();

    void htmlfull(stringstream &html);

};

