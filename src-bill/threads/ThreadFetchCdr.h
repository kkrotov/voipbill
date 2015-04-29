#pragma once

#include "../../src/threads/Thread.h"
#include "../data/DataBillingContainer.h"
#include "../classes/CdrLoader.h"

class ThreadFetchCdr : public Thread {
protected:
    BDb db_calls;
    DataBillingContainer * billingData;
    CdrLoader cdrLoader;


public:
    bool prepare();

    void run();

    void htmlfull(stringstream &html);
public:
    ThreadFetchCdr();
    static const char* idName() { return "fetch_cdr"; }
};

