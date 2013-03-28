#pragma once

#include <time.h>
#include <map>
#include <string>
using namespace std;

#include <boost/interprocess/sync/interprocess_mutex.hpp>
using boost::interprocess::interprocess_mutex;


class BlackList
{
protected:

    time_t last_fetch_from_openca_time;

    void log_lock_phone(string &phone);
    void log_unlock_phone(string &phone);

public:
    map<long long int,time_t> blacklist;
    map<long long int,bool> list_to_add;
    map<long long int,bool> list_to_del;

    interprocess_mutex lock;

    BlackList();

    static BlackList * instance(){
        static BlackList inst;
        return &inst;
    }

    void add(long long int phone);
    void del(long long int phone);

    bool is_locked(long long int phone);

    bool fetch();
    bool fetch_once_per_day();
    void push();
};
