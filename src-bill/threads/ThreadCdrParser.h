#pragma once

#include "../classes/Thread.h"
#include "../classes/BDb.h"
#include <list>

using namespace std;

struct CdrFile {
    std::string file_name;
    size_t total_count;
    size_t insert_count;
    size_t error_count;
    string error_ids;
};

class ThreadCdrParser : public Thread {
private:
    BDb db_calls;
    size_t files_on_server_count;
    size_t processed_files_count;
    size_t processed_calls_count;
    string current_file_name;
    size_t current_file_calls_count;
    string parser_log;
    list<CdrFile> last_files;
public:
    ThreadCdrParser();
    void run();

    bool hasFullHtml() override;
    void htmlfull(stringstream &html);

    static const char* idName() { return "cdr_parser"; };
private:
    void LogBillingQuery(int Reason, unsigned int ClientID);
    bool isFileProcessed(const std::string &Filename);
};
