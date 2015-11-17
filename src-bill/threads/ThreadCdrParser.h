#pragma once

#include "../classes/Thread.h"
#include "../classes/BDb.h"

class ThreadCdrParser : public Thread {
private:
    BDb db_calls;
public:
    ThreadCdrParser();
    bool prepare();
    void run();
    static const char* idName() { return "cdr_parser"; };
private:
    void LogBillingQuery(int Reason, unsigned int ClientID);
    bool isFileProcessed(const std::string &Filename);
};
