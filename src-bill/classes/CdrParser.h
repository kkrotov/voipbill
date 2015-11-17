#pragma once

#include <string>
#include <list>

struct CallData
{
    unsigned long call_id;
    std::string src_number;
    std::string dst_number;
    std::string redirect_number;
    std::string  setup_time;
    std::string  connect_time;
    std::string  disconnect_time;
    unsigned long session_time;
    std::string src_route;
    std::string dst_route;
    unsigned int src_noa;
    unsigned int dst_noa;
    int disconnect_cause;
    std::string setup_time_raw;
};

class CdrParser {
private:
    typedef std::list<CallData> CallDataList;
    std::string rawdata;
    unsigned int errorcallscount;
    std::string errorids;
public:
    CdrParser();
    CdrParser(const std::string &RawData);
    bool Parse(CallDataList &Calls);
    unsigned int GetErrorCallsCount();
    std::string GetErrorIDs();
private:
    bool ParseCall(const std::string &RawCallData, CallData &Call);
    std::string GetValue(const std::string &Name, const std::string &Data);
    std::string ParseDateTime(const std::string &Data, bool CanBeEmpty = false);
    void ParseNumber(const std::string &Data, std::string &Number, unsigned int &Noa, bool CanBeEmpty = false);
    unsigned long ParseSessionTime(const std::string &Data);
    int ParseDisconnectCause(const std::string &Data);

};
