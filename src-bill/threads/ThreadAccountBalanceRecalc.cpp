#include "ThreadAccountBalanceRecalc.h"

void ThreadAccountBalanceRecalc::run() {

    Repository repository;

    if (!repository.prepare()) {
        return;
    }

    if (!repository.billingData->ready()) {
        return;
    }

    list<int> accountIds;
    repository.billingData->getAccountIdsForRecalcAccountSum(repository.data, accountIds);

    {
        lock_guard<Spinlock> guard(lock);
        accountIdsForRecalc = accountIds;
    }

    repository.billingData->recalcAccountSum(&db_calls, accountIds);


    {
        lock_guard<Spinlock> guard(lock);
        total_recalced_accounts += accountIdsForRecalc.size();
        accountIdsForRecalc.clear();
    }
}


bool ThreadAccountBalanceRecalc::hasFullHtml() {
    return true;
}

void ThreadAccountBalanceRecalc::htmlfull(stringstream &html) {
    this->html(html);

    lock_guard<Spinlock> guard(lock);

    html << "total recalced accounts: <b>" << total_recalced_accounts << "</b><br/>\n";

    html << "accounts for recalc count: <b>" << accountIdsForRecalc.size() << "</b><br/>\n";
    html << "<br/>\n";
    for (int accountId : accountIdsForRecalc) {
        html << "<a href='/client?id=" << accountId << "'>" << accountId << "</a><br/>\n";
    }
}


ThreadAccountBalanceRecalc::ThreadAccountBalanceRecalc() {
    id = idName();
    name = "Account balance recalc";


    db_calls.setCS(app().conf.db_calls);
}
