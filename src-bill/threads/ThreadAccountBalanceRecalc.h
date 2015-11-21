#pragma once

#include "../classes/Thread.h"
#include "../classes/Repository.h"

class ThreadAccountBalanceRecalc : public Thread {
    Spinlock lock;
    Repository repository;
    BDb db_calls;
    list<int> accountIdsForRecalc;
    size_t total_recalced_accounts = 0;
    void run();

public:

    virtual bool hasFullHtml() override;

    virtual void htmlfull(stringstream &html) override;


    ThreadAccountBalanceRecalc();
    static const char* idName() { return "account_balance_recalc"; }
};
