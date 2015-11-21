#pragma once

#include "../classes/Thread.h"
#include "../classes/Repository.h"

class ThreadClientLock : public Thread {

    Repository repository;

    bool ready();

    void run();

    bool needLockLocal(int client_account_id);
    bool needLockGlobal(int client_account_id);


public:
    virtual bool hasFullHtml() override;
    virtual void htmlfull(stringstream &html) override;

public:
    ThreadClientLock();
    static const char* idName() { return "client_lock"; }
};
