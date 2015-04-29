#pragma once

#include <time.h>
#include <map>
#include <string>
#include <mutex>
#include <vector>

using namespace std;

class BlackList {
protected:
    void log_lock_phone(string &phone);
    void log_unlock_phone(string &phone);

    virtual bool udp_blacklist(vector<string> &list) = 0;
    virtual bool udp_lock(string &phone) = 0;
    virtual bool udp_unlock(string &phone) = 0;

public:
    map<string, time_t> blacklist;
    map<string, bool> list_to_add;
    map<string, bool> list_to_del;

    mutex rwlock;

    BlackList();

    void add(const string &phone);
    void del(const string &phone);

    void change_lock(const string &phone, bool need_lock);
    void lock(const string &phone);
    void unlock(const string &phone);

    bool is_locked(const string &phone);

    bool fetch();
    void push();
};
