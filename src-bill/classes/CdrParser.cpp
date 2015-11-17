#include "CdrParser.h"
#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>
#include "../classes/LogWriterSyslog.h"
#include "../classes/Log.h"
#include <boost/lexical_cast.hpp>
#include <regex>

using namespace std;

CdrParser::CdrParser() {

}

CdrParser::CdrParser(const std::string &RawData) {
    rawdata = RawData;
}

bool CdrParser::Parse(CallDataList &Calls) {
    try {
        errorcallscount = 0;
        errorids.clear();
        boost::replace_all(rawdata, "\r", "");
        boost::replace_all(rawdata, "\n", "");
        const std::string callopentagname("<OT_Call_CDR>");
        const std::string callclosetagname("</OT_Call_CDR>");
        unsigned long beginpos(0);
        unsigned long datalength(0);
        unsigned long nextcallrecordposition(0);
        int counter(0);
        while (nextcallrecordposition != std::string::npos) {
            ++counter;
            unsigned long callrecordposition = rawdata.find(callopentagname, beginpos + datalength);

            nextcallrecordposition = rawdata.find(callopentagname, callrecordposition + callopentagname.length());
            unsigned long callcloserecordposition = rawdata.find(callclosetagname, callrecordposition + callopentagname.length());

            // Нормальная ситуация если nextcallcloserecordposition > callcloserecordposition
            beginpos = callrecordposition + callopentagname.length();
            datalength = callcloserecordposition - beginpos;

            // Обработка если нет закрытого тэга перед открытым, но звонок не последний
            if ((string::npos != nextcallrecordposition) && (nextcallrecordposition < callcloserecordposition)) {
                datalength = nextcallrecordposition - beginpos;
            }
            // Обработка если звонок поледний в списке и тег не закрыт
            if ((string::npos == nextcallrecordposition) && (string::npos == callcloserecordposition)) {
                datalength = string::npos;
            }
            std::string rawcalldata;
            rawcalldata = rawdata.substr(beginpos, datalength);
            CallData calldata;
            if(ParseCall(rawcalldata, calldata)) {
                Calls.push_back(calldata);
            } else {
                errorcallscount++;
                errorids += (boost::format("%1%, ") % calldata.call_id).str();
            }
        }
    } catch (Exception &e) {
        Log::error((boost::format("CDR file parsing fatal error. Reason: %1%") % e.what()).str());
        return false;
    }
    return true;
}

std::string CdrParser::ParseDateTime(const std::string &Data, bool CanBeEmpty)
{
    if(CanBeEmpty && (Data.size() == 0))
        return "";
    std::string result = (boost::format("%1%-%2%-%3% %4%:%5%:%6%.%7%+00") % Data.substr(0, 4) % Data.substr(4, 2) %
              Data.substr(6, 2) % Data.substr(8, 2) % Data.substr(10, 2) % Data.substr(12, 2) % Data.substr(15, 6)).str();
    return result;
}

void CdrParser::ParseNumber(const std::string &Data, std::string &Number, unsigned int &Noa, bool CanBeEmpty) {
    if((Data.size() > 0 ) || !CanBeEmpty) {
        unsigned long spacepos = Data.find(' ');
        if(std::string::npos != spacepos) {
            Number = Data.substr(0, spacepos);
            Noa = boost::lexical_cast<unsigned int>(Data.substr(spacepos + 1));

        } else {
            Number = Data;
            Noa = 3;
        }
    } else {
        Number = "anonymous";
        Noa = 3;
    }
}

unsigned long CdrParser::ParseSessionTime(const std::string &Data) {
    unsigned long callduration(0);
    if(Data.length() != 0) {
        unsigned long dividerpos = Data.find(':');
        if(std::string::npos != dividerpos) {
            callduration = boost::lexical_cast<unsigned long>(Data.substr(0, dividerpos)) + 1;
        } else {
            callduration = boost::lexical_cast<unsigned long>(Data) + 1;
        }
    }
    return callduration;
}

int CdrParser::ParseDisconnectCause(const std::string &Data) {
    int resultcode(0);
    if(Data.size() >= 0) {
        unsigned long startpos = Data.find(':');
        if(std::string::npos != startpos) {
            unsigned long endpos = Data.find(' ', startpos + 1);
            std::string strresultcode = Data.substr(startpos + 1, endpos - startpos - 1);
            resultcode = boost::lexical_cast<int>(strresultcode);
        }
    }
    return resultcode;
}

bool CdrParser::ParseCall(const std::string &RawCallData, CallData &Call) {
    Call.call_id = 0;
    std::string datatagname;
    std::string value;
    // Имена значений инициализируются заранее чтобы в случае выброса исключения вывести их в системный лог. Это нужно чтобы не отвлекать программистов на отдельный прогон неразобранного CDR участка для обнаружения проблемы.
    try {
        datatagname = "CallReference";
        Call.call_id = boost::lexical_cast<unsigned long>(GetValue(datatagname, RawCallData));
        datatagname = "IncomingCallingAddress";
        value = GetValue(datatagname, RawCallData);
        ParseNumber(value, Call.src_number, Call.src_noa, true);
        datatagname = "IncomingCalledAddress";
        value = GetValue(datatagname, RawCallData);
        ParseNumber(value, Call.dst_number, Call.dst_noa);
        datatagname = "IncomingRedirectingAddress";
        value = GetValue(datatagname, RawCallData);
        unsigned int dummy;
        ParseNumber(value, Call.redirect_number, dummy, true);
        datatagname = "StartTimeStamp";
        value = GetValue(datatagname, RawCallData);
        Call.setup_time = ParseDateTime(value);
        datatagname = "AnswerTimeStamp";
        value = GetValue(datatagname, RawCallData);
        Call.connect_time = ParseDateTime(value, true);
        datatagname = "ReleaseTimeStamp";
        value = GetValue(datatagname, RawCallData);
        Call.disconnect_time = ParseDateTime(value, true);
        datatagname = "CallDuration";
        value = GetValue(datatagname, RawCallData);
        Call.session_time = ParseSessionTime(value);
        datatagname = "IncomingRouteId";
        Call.src_route = GetValue(datatagname, RawCallData);
        datatagname = "OutgoingRouteId";
        Call.dst_route = GetValue(datatagname, RawCallData);
        datatagname = "CauseIndicator";
        value = GetValue(datatagname, RawCallData);
        Call.disconnect_cause = ParseDisconnectCause(value);
        datatagname = "StartTimeStamp";
        Call.setup_time_raw = GetValue(datatagname, RawCallData);
    } catch(...) {
        std::string errstr = (boost::format("Call %1% parsing error for parameter %2% with value %3%") % Call.call_id % datatagname % value).str();
        LogWriterSyslog syslog(errstr, LogLevel::ERROR);
        Log::error(errstr);
        return false;
    }
    return true;
}

std::string CdrParser::GetValue(const std::string &Name, const std::string &Data) {
    std::string value;
    std::string searchstring = (boost::format("%1%>(.+)</%1%") % Name).str();
    std::regex re(searchstring);
    std::smatch match;
    if (std::regex_search(Data, match, re) && (match.size() >= 2)) {
        value = match[1];
    }
    return value;
}

unsigned int CdrParser::GetErrorCallsCount() {
    return errorcallscount;
}

std::string CdrParser::GetErrorIDs() {
    return errorids;
}