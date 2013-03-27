#pragma once

#include "Task.h"
#include "../DataLoader.h"
#include "../BlackList.h"

class TaskBlacklist : public Task
{
    Timer t;
    DataLoader *loader;
    BlackList *blacklist;

    void wait();
    void prepare();

    void run();

    void update_voip_auto_disabled();


    void htmlfull(stringstream &html);
public:
    TaskBlacklist();
};
