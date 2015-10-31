#pragma once

#include "../classes/Thread.h"
#include "../classes/BDb.h"

struct CallData
{
    unsigned long call_id;
    std::string src_number;
    unsigned int src_noa;
    std::string dst_number;
    unsigned int dst_noa;
    std::string redirect_number;
    std::string  setup_time;
    std::string  connect_time;
    std::string  disconnect_time;
    unsigned long session_time;
    std::string src_route;
    std::string dst_route;
    int disconnect_cause;
    std::string setup_time_raw;
};

class CdrParser {
private:
    typedef std::list<CallData> CallDataList;
    std::string rawdata;
public:
    CdrParser();
    CdrParser(const std::string &RawData);
    bool Parse(CallDataList &Calls);
    bool Parse(const std::string &RawData, CallDataList &Calls);
private:
    bool ParseCall(const std::string &RawCallData, CallData &Call);
    std::string GetValue(const std::string &Name, const std::string &Data);
    std::string ParseDateTime(const std::string &Data, bool CanBeEmpty = false);
    void ParseNumber(const std::string &Data, std::string &Number, unsigned int &Noa, bool CanBeEmpty = false);
    unsigned long ParseSessionTime(const std::string &Data);
    int ParseDisconnectCause(const std::string &Data);

};

class ThreadCdrParser : public Thread {
private:
    BDb db_calls;
public:
    ThreadCdrParser();
    bool prepare();
    void run();
    static const char* idName() { return "cdr_parser"; };
private:
    bool isFileProcessed(const std::string &Filename);
    bool ProcessData(const std::string &Data);
};
