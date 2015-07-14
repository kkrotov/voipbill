#include "DataCurrentCallsContainer.h"

DataCurrentCallsContainer::DataCurrentCallsContainer() {
    callsWaitSaving = shared_ptr<vector<Call>>(new vector<Call>());
    statsPackagesCounter = shared_ptr<StatsPackageCounter>(new StatsPackageCounter());
    clientCounter = shared_ptr<ClientCounter>(new ClientCounter());
    fminCounter = shared_ptr<FminCounter>(new FminCounter());
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

void DataCurrentCallsContainer::setClientCounter(shared_ptr<ClientCounter> &newClientCounter) {
    lock_guard<Spinlock> guard(lock);
    clientCounter = newClientCounter;
}

shared_ptr<ClientCounter> DataCurrentCallsContainer::getClientCounter() {
    lock_guard<Spinlock> guard(lock);
    return clientCounter;
}

void DataCurrentCallsContainer::setFminCounter(shared_ptr<FminCounter> &newFminCounter) {
    lock_guard<Spinlock> guard(lock);
    fminCounter = newFminCounter;
}

shared_ptr<FminCounter> DataCurrentCallsContainer::getFminCounter() {
    lock_guard<Spinlock> guard(lock);
    return fminCounter;
}

void DataCurrentCallsContainer::setStatsPackagesCounter(shared_ptr<StatsPackageCounter> &newStatsPackagesCounter) {
    lock_guard<Spinlock> guard(lock);
    statsPackagesCounter = newStatsPackagesCounter;
}

shared_ptr<StatsPackageCounter> DataCurrentCallsContainer::getStatsPackagesCounter() {
    lock_guard<Spinlock> guard(lock);
    return statsPackagesCounter;
}

