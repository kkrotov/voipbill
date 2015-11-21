#include "DataCurrentCallsContainer.h"

DataCurrentCallsContainer::DataCurrentCallsContainer() {
    callsWaitSaving = shared_ptr<vector<Call>>(new vector<Call>());
    statsAccount = shared_ptr<StatsAccountManager>(new StatsAccountManager());
    statsFreemin = shared_ptr<StatsFreeminManager>(new StatsFreeminManager());
    statsPackage = shared_ptr<StatsPackageManager>(new StatsPackageManager());
}

DataCurrentCallsContainer * DataCurrentCallsContainer::instance() {
    static DataCurrentCallsContainer inst;
    return &inst;
}

bool DataCurrentCallsContainer::ready() {

    if (!currentCdr.ready()) {
        return false;
    }

    return true;
}

void DataCurrentCallsContainer::setCallsWaitingSaving(shared_ptr<vector<Call>> &newCallsWaitSaving) {
    lock_guard<Spinlock> guard(lock);
    callsWaitSaving = newCallsWaitSaving;
}

shared_ptr<vector<Call>> DataCurrentCallsContainer::getCallsWaitingSaving() {
    lock_guard<Spinlock> guard(lock);
    return callsWaitSaving;
}

void DataCurrentCallsContainer::setStatsAccount(shared_ptr<StatsAccountManager> &newStatsAccount) {
    lock_guard<Spinlock> guard(lock);
    statsAccount = newStatsAccount;
}

shared_ptr<StatsAccountManager> DataCurrentCallsContainer::getStatsAccount() {
    lock_guard<Spinlock> guard(lock);
    return statsAccount;
}

void DataCurrentCallsContainer::setStatsFreemin(shared_ptr<StatsFreeminManager> &newStatsFreemin) {
    lock_guard<Spinlock> guard(lock);
    statsFreemin = newStatsFreemin;
}

shared_ptr<StatsFreeminManager> DataCurrentCallsContainer::getStatsFreemin() {
    lock_guard<Spinlock> guard(lock);
    return statsFreemin;
}

void DataCurrentCallsContainer::setStatsPackage(shared_ptr<StatsPackageManager> &newStatsPackage) {
    lock_guard<Spinlock> guard(lock);
    statsPackage = newStatsPackage;
}

shared_ptr<StatsPackageManager> DataCurrentCallsContainer::getStatsPackage() {
    lock_guard<Spinlock> guard(lock);
    return statsPackage;
}

