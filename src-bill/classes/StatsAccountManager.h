#pragma once

#include "../common.h"
#include "BDb.h"
#include "Spinlock.h"
#include "../models/CallInfo.h"
#include "../models/StatsAccount.h"

class DataBillingContainer;

class StatsAccountManager
{
private:
    vector<map<int, StatsAccount>> realtimeStatsAccountParts;
    map<int, StatsAccount> statsAccount;
    set<int> forSync;

    bool loaded = false;
public:
    bool needClear = false;
    StatsAccountManager();
    bool ready() { return loaded; };
    void load(BDb * db);
    void recalc(BDb * db);
    void reloadSum(BDb * db, list<int> accountIds, Spinlock &lock);

    void prepareSaveQuery(stringstream &query);
    void executeSaveQuery(BDb * dbCalls, stringstream &query);

    void createNewPartition();
    void removePartitionAfterSave();

    double getSumMonth(int account_id, double vat_rate);
    double getSumDay(int account_id, double vat_rate);
    double getSumBalance(int account_id, double vat_rate);

    void getClients(vector<StatsAccount> &destClients) {
        destClients.reserve(statsAccount.size());
        for (auto pair : statsAccount) {
            destClients.push_back(pair.second);
        }
    }

    void getChanges(map<int, StatsAccount> &changes, bool &needClear);
    void addChanges(map<int, StatsAccount> &changes);

    StatsAccount * get(int account_id);

    size_t sync(BDb * db_main, DataBillingContainer * billingData);

private:
    void add(CallInfo * callInfo);
    size_t size();
    friend class DataBillingContainer;
    friend class Billing;
};