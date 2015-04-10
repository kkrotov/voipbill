#pragma once

#include "ManagerPull.h"
#include "../../src/classes/Timer.h"

#include <string>
#include <vector>
#include <set>

using namespace std;

class ManagerPull;

class BasePull {
public:
    Timer timer;
    string event;
    int pull_count_full;
    int pull_count_updated;
    int pull_count_deleted;
    int pull_count_errors;

    virtual void init() = 0;
    void pull();
    void clear();
    void addId(string &id);
    void setManager(ManagerPull * manager);

    BasePull();

protected:
    ManagerPull * manager;
    string src_table;
    string dst_table;
    string key;
    vector<string> fields;

    set<string> ids_to_pull;
    bool need_pull;
    bool full_pull;

    void pullFull();
    void pullPartial();

    string getQueryFields();
    string getFilterIds();
};
