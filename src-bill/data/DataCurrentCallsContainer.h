#pragma once

#include "../../src/classes/BDb.h"
#include "../../src/common.h"
#include "../models/Cdr.h"
#include "../models/Call.h"
#include "../lists/ClientCounter.h"
#include "../lists/FminCounter.h"
#include "CurrentCdrData.h"

class DataCurrentCallsContainer {
public:
    CurrentCdrData currentCdr;

    long long int lastCallId = 0;
    int callPerSecond = -1;
    int maxCallPerSecond = -1;

    DataCurrentCallsContainer() {
        callsWaitSaving = shared_ptr<vector<Call>>(new vector<Call>());
        clientCounter = shared_ptr<ClientCounter>(new ClientCounter());
        fminCounter = shared_ptr<FminCounter>(new FminCounter());
    }

    static DataCurrentCallsContainer * instance() {
        static DataCurrentCallsContainer inst;
        return &inst;
    }

    void setCallsWaitingSaving(shared_ptr<vector<Call>> &newCallsWaitSaving) {
        lock_guard<Spinlock> guard(callsWaitSavingLock);
        callsWaitSaving = newCallsWaitSaving;
    }

    shared_ptr<vector<Call>> getCallsWaitingSaving() {
        lock_guard<Spinlock> guard(callsWaitSavingLock);
        return callsWaitSaving;
    }

    void setClientCounter(shared_ptr<ClientCounter> &newClientCounter) {
        lock_guard<Spinlock> guard(clientCounterLock);
        clientCounter = newClientCounter;
    }

    shared_ptr<ClientCounter> getClientCounter() {
        lock_guard<Spinlock> guard(clientCounterLock);
        return clientCounter;
    }

    void setFminCounter(shared_ptr<FminCounter> &newFminCounter) {
        lock_guard<Spinlock> guard(fminCounterLock);
        fminCounter = newFminCounter;
    }

    shared_ptr<FminCounter> getFminCounter() {
        lock_guard<Spinlock> guard(fminCounterLock);
        return fminCounter;
    }


    bool ready() {

        if (!currentCdr.ready()) {
            return false;
        }

        return true;
    }

private:
    Spinlock callsWaitSavingLock;
    shared_ptr<vector<Call>> callsWaitSaving;
    Spinlock clientCounterLock;
    shared_ptr<ClientCounter> clientCounter;
    Spinlock fminCounterLock;
    shared_ptr<FminCounter> fminCounter;
};