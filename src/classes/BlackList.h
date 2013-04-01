#pragma once

#include <time.h>
#include <map>
#include <string>
#include <mutex>
#include <vector>

using namespace std;

class BlackList
{
protected:
    void log_lock_phone(string &phone);
    void log_unlock_phone(string &phone);

    virtual bool udp_blacklist(vector<string> &list) = 0;
    virtual bool udp_lock(string &phones) = 0;
    virtual bool udp_unlock(string &phones) = 0;

public:
    map<long long int,time_t> blacklist;
    map<long long int,bool> list_to_add;
    map<long long int,bool> list_to_del;

    mutex lock;

    BlackList();

    void add(long long int phone);
    void del(long long int phone);

    bool is_locked(long long int phone);

    bool fetch();
    void push();
};
