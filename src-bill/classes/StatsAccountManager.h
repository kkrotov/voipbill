#pragma once

#include "../common.h"
#include "BDb.h"
#include "Spinlock.h"
#include "../models/CallInfo.h"
#include "../models/StatsAccount.h"

class StatsAccountManager
{
private:
    long long int lastStoredCallTime;

    map<int, StatsAccount> storedStatsAccount;
    vector<map<int, StatsAccount>> realtimeStatsAccountParts;

    map<int, StatsAccount> statsAccount;

    bool loaded = false;
public:
    StatsAccountManager();
    bool ready() { return loaded; };
    void load(BDb * db);
    void reload(BDb * db);

    void prepareSaveQuery(stringstream &query);
    void executeSaveQuery(BDb * dbCalls, stringstream &query);

    void createNewPartition();
    void removePartitionAfterSave();

    double getSumMonth(int account_id, double vat_rate);
    double getSumDay(int account_id, double vat_rate);
    double getSumBalance(int account_id, double vat_rate);

    void getClients(vector<StatsAccount> &destClients) {
        destClients.reserve(storedStatsAccount.size());
        for (auto pair : storedStatsAccount) {
            destClients.push_back(pair.second);
        }
    }

private:
    void add(CallInfo * callInfo);
    size_t size();
    friend class DataBillingContainer;
    friend class Billing;
};