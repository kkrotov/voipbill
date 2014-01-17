#pragma once

#include <string>

#include "../classes/BDb.h"
#include "../common.h"

using namespace std;

class Task {
protected:
    BDb *db_main;
    string id;
    string params;
    string status;
    string name;
    spinlock status_lock;
public:
    void setStatus(string str);
    string getStatus();
    string getParams();
    string getName();

    void initTask(BDb &db_main, string id, string params);

    virtual void html(stringstream &html);

    virtual void run() = 0;
};

