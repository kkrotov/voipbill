#pragma once

#include <time.h>
#include <map>
#include <set>
#include <string>
#include "Spinlock.h"
#include <vector>
#include "Log.h"
#include "Repository.h"

using namespace std;

class BlackList {
protected:
    void log_lock_phone(const string &phone);
    void log_unlock_phone(const string &phone);
    void log_info(const string &phone, pLogMessage &logRequest);
    void fetchGlobalCounters(Repository &repository, int accountId, double &globalBalanceSum, double &globalDaySum, double vat_rate);

    virtual bool udp_blacklist(vector<string> &list) = 0;
    virtual bool udp_lock(const string &phone) = 0;
    virtual bool udp_unlock(const string &phone) = 0;

public:
    set<string> blacklist;
    set<string> list_to_add;
    set<string> list_to_del;

    Spinlock lock;

    bool fetch();
    void push(set<string> &wanted_blacklist, bool is_trunk);
};
