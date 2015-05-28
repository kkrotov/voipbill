#pragma once

#include <time.h>
#include <map>
#include <set>
#include <string>
#include "../../src/classes/Spinlock.h"
#include <vector>

using namespace std;

class BlackList {
protected:
    void log_lock_phone(const string &phone);
    void log_unlock_phone(const string &phone);

    virtual bool udp_blacklist(vector<string> &list) = 0;
    virtual bool udp_lock(const string &phone) = 0;
    virtual bool udp_unlock(const string &phone) = 0;

public:
    set<string> blacklist;
    set<string> list_to_add;
    set<string> list_to_del;

    Spinlock lock;

    bool fetch();
    void push(set<string> &wanted_blacklist);
};
