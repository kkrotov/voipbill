#pragma once

#include "../classes/Thread.h"
#include "../classes/BDb.h"
#include "../classes/FtpClient.h"
#include "../classes/CdrParser.h"
#include <list>

using namespace std;

struct CdrFile {
    std::string file_name;
    size_t total_count;
    size_t insert_count;
    size_t error_count;
    string error_ids;
    size_t progress;

    void clear() {
        file_name = "";
        total_count = 0;
        insert_count = 0;
        error_count = 0;
        error_ids = "";
        progress = 0;
    }
};

class ThreadCdrParser : public Thread {
private:
    FtpClient ftpClient;
    CdrParser parser;
    CdrFile cdrFile;
    BDb db_calls;
    size_t files_total;
    size_t files_progres;
    string parser_log;
    list<CdrFile> last_files;
public:
    ThreadCdrParser();
    void run();

    bool hasFullHtml() override;
    void htmlfull(stringstream &html);

    static const char* idName() { return "cdr_parser"; };
private:
    void processFile(const string &fileName);
    void fetchFile(string &fileContent);
    bool parseFile(std::list<CallData> &calls, const string &fileContent);
    void saveCalls(const std::list<CallData> &calls);
    void saveCall(const CallData &call);
    void saveUnfinishedCall(const CallData &call);
    void logCall(CallData &call);
    bool isCallExists(const string &hash);
    bool isUnfinishedCallExists(const string &hash, string dst_route);
    void saveCdrFile();
    string getFileNameForParse();
    bool isFileProcessed(const std::string &Filename);
    void logParser(const std::string &message);
};
