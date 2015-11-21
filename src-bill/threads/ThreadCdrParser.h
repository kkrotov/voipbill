#pragma once

#include "../classes/Thread.h"
#include "../classes/BDb.h"

class ThreadCdrParser : public Thread {
private:
    BDb db_calls;
    size_t files_on_server_count;
    size_t processed_files_count;
    size_t processed_calls_count;
    string last_file_name;
    size_t last_file_calls_count;
public:
    ThreadCdrParser();
    bool prepare();
    void run();

    bool hasFullHtml() override;
    void htmlfull(stringstream &html);

    static const char* idName() { return "cdr_parser"; };
private:
    void LogBillingQuery(int Reason, unsigned int ClientID);
    bool isFileProcessed(const std::string &Filename);
};
