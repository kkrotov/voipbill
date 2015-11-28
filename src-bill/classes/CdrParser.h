#pragma once

#include <string>
#include <list>

struct CallData
{
    std::string call_id;
    std::string src_number;
    std::string dst_number;
    std::string redirect_number;
    std::string dst_replace;
    std::string setup_time;
    std::string connect_time;
    std::string disconnect_time;
    unsigned long session_time;
    std::string src_route;
    std::string dst_route;
    unsigned int src_noa;
    unsigned int dst_noa;
    int disconnect_cause;
    std::string setup_time_raw;
    std::string hash;
};

class CdrParser {
private:
    std::string rawdata;
    unsigned int errorcallscount;
    std::string errorids;
public:
    CdrParser();
    bool Parse(std::list<CallData> &Calls, const std::string &RawData);
    unsigned int GetErrorCallsCount();
    std::string GetErrorIDs();
private:
    bool ParseCall(const std::string &RawCallData, CallData &Call);
    std::string GetValue(const std::string &Name, const std::string &Data, bool CanBeEmpty = false);
    std::string ParseDateTime(const std::string &Name, const std::string &Data, bool canBeEmpty = false);
    void ParseNumber(const std::string &Name, const std::string &Data, std::string &Number, unsigned int &Noa, bool CanBeEmpty = false);
    unsigned long ParseSessionTime(const std::string &Name, const std::string &Data);
    int ParseDisconnectCause(const std::string &Name, const std::string &Data);
    void CalculateHash(CallData &call);
};
