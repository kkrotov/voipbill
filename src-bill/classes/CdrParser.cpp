#include "CdrParser.h"
#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>
#include "../classes/Log.h"
#include <boost/lexical_cast.hpp>
#include <regex>
#include <openssl/md5.h>

using namespace std;

CdrParser::CdrParser() {

}

bool CdrParser::Parse(std::list<CallData> &Calls, const std::string &RawData) {
    try {
        rawdata = RawData;
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
            if (ParseCall(rawcalldata, calldata)) {
                Calls.push_back(calldata);
            }
        }
    } catch (Exception &e) {
        Log::error((boost::format("CDR file parsing fatal error. Reason: %1%") % e.what()).str());
        return false;
    }
    return true;
}

std::string CdrParser::ParseDateTime(const std::string &Name, const std::string &Data,  bool CanBeEmpty)
{
    std::string value = GetValue(Name, Data, CanBeEmpty);
    if (CanBeEmpty && (value.size() == 0))
        return "";
    std::string result = (boost::format("%1%-%2%-%3% %4%:%5%:%6%.%7%+00") % value.substr(0, 4) % value.substr(4, 2) %
            value.substr(6, 2) % value.substr(8, 2) % value.substr(10, 2) % value.substr(12, 2) % value.substr(15, 6)).str();
    return result;
}

void CdrParser::ParseNumber(const std::string &Name, const std::string &Data, std::string &Number, unsigned int &Noa, bool CanBeEmpty) {
    std::string value = GetValue(Name, Data, CanBeEmpty);

    if((value.size() > 0 ) || !CanBeEmpty) {
        unsigned long spacepos = value.find(' ');
        if(std::string::npos != spacepos) {
            Number = value.substr(0, spacepos);
            Noa = boost::lexical_cast<unsigned int>(value.substr(spacepos + 1));
        } else {
            Number = value;
            Noa = 3;
        }
    } else {
        Number = "anonymous";
        Noa = 3;
    }

    if (Number.size() > 32) {
        Number = Number.substr(0, 32);
    }
}

unsigned long CdrParser::ParseSessionTime(const std::string &Name, const std::string &Data) {
    std::string value = GetValue(Name, Data, true);

    unsigned long callduration(0);
    if (value.length() != 0) {
        unsigned long dividerpos = value.find(':');
        if(std::string::npos != dividerpos) {
            callduration = boost::lexical_cast<unsigned long>(value.substr(0, dividerpos)) + 1;
        } else {
            callduration = boost::lexical_cast<unsigned long>(value) + 1;
        }
    }
    return callduration;
}

int CdrParser::ParseDisconnectCause(const std::string &Name, const std::string &Data) {
    std::string value = GetValue(Name, Data, true);

    int resultcode(0);
    if(value.size() >= 0) {
        unsigned long startpos = value.find(':');
        if(std::string::npos != startpos) {
            unsigned long endpos = value.find(' ', startpos + 1);
            std::string strresultcode = value.substr(startpos + 1, endpos - startpos - 1);
            resultcode = boost::lexical_cast<int>(strresultcode);
        }
    }
    return resultcode;
}

bool CdrParser::ParseCall(const std::string &RawCallData, CallData &Call) {
    unsigned int dummy_noa;

    try {
        Call.call_finished = GetValue("CallFinished", RawCallData);
        bool call_is_finished = Call.IsFinished();

        Call.call_id = GetValue("CallReference", RawCallData);

        ParseNumber("IncomingCallingAddress", RawCallData, Call.src_number, Call.src_noa, true);

        ParseNumber("IncomingCalledAddress", RawCallData, Call.dst_number, Call.dst_noa, !call_is_finished);

        ParseNumber("IncomingRedirectingAddress", RawCallData, Call.redirect_number, dummy_noa, true);

        ParseNumber("OutgoingCalledAddress", RawCallData, Call.dst_replace, dummy_noa, true);

        Call.setup_time = ParseDateTime("StartTimeStamp", RawCallData, !call_is_finished);

        Call.connect_time = ParseDateTime("AnswerTimeStamp", RawCallData, true);

        Call.disconnect_time = ParseDateTime("ReleaseTimeStamp", RawCallData, !call_is_finished);

        Call.session_time = ParseSessionTime("CallDuration", RawCallData);

        Call.src_route = GetValue("IncomingRouteId", RawCallData, !call_is_finished);

        Call.dst_route = GetValue("OutgoingRouteId", RawCallData, true);

        Call.disconnect_cause = ParseDisconnectCause("CauseIndicator", RawCallData);

        Call.setup_time_raw = GetValue("StartTimeStamp", RawCallData, !call_is_finished);

        Call.releasing_party = GetValue("ReleasingParty", RawCallData, true);

        CalculateHash(Call);

    } catch(Exception &e) {
        Log::error("Call parsing error: " + e.message + "\n" + RawCallData);
        errorcallscount++;
        errorids += (boost::format("%1%, ") % Call.call_id).str();
        return false;
    } catch(...) {
        Log::error("Call parsing error\n" + RawCallData);
        errorcallscount++;
        errorids += (boost::format("%1%, ") % Call.call_id).str();
        return false;
    }
    return true;
}

std::string CdrParser::GetValue(const std::string &Name, const std::string &Data, bool CanBeEmpty) {
    std::string value;
    std::string searchstring = (boost::format("%1%>(.*)</%1%") % Name).str();
    std::regex re(searchstring);
    std::smatch match;
    if (std::regex_search(Data, match, re) && (match.size() >= 2)) {
        value = (string)match[1];
    } else {
        if (!CanBeEmpty) {
            Log::error("Call parsing error: Call field " + Name + " not found\n" + Data);
        }
    }
    return value;
}

unsigned int CdrParser::GetErrorCallsCount() {
    return errorcallscount;
}

std::string CdrParser::GetErrorIDs() {
    return errorids;
}

void CdrParser::CalculateHash(CallData &call) {
    std::string base = call.call_id + call.setup_time_raw.substr(0, 14);

    unsigned char result[MD5_DIGEST_LENGTH];
    MD5((unsigned char *)base.c_str(), base.size(), result);

    call.hash = "";
    char buffer[10];

    for(int i=0; i <MD5_DIGEST_LENGTH; i++) {
        sprintf(buffer, "%02x", result[i]);
        call.hash += buffer;
    }
}