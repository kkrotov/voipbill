#pragma once

#include "../common.h"
#include "../classes/DbException.h"
#include "../classes/Timer.h"
#include "../classes/Log.h"
#include "../classes/App.h"

class Task
{
public:
    string id;
    string name;
    string status;

    Task();

    void start();

    static void ssleep(long seconds);

    virtual void wait();

    virtual void prepare();

    virtual void run() = 0;

    void operator()();

    void html(stringstream &html)
    {
        html << "<h3><a href='/task?id=" << this->id << "'>" << this->name << "</a> " << status << "</h3>\n";
    }
    virtual void htmlfull(stringstream &html) {
        this->html(html);
    }
};

