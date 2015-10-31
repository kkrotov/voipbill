#include "ThreadCdrParser.h"
#include "../classes/AppBill.h"
#include <boost/format.hpp>
#include <boost/foreach.hpp>
#include "../classes/FTPClient.h"

ThreadCdrParser::ThreadCdrParser() : Thread()
{
    name ="CDR Parser";
    id = idName();
    threadSleepSeconds = 60000;
    db_calls.setCS(app().conf.db_calls);
}

bool ThreadCdrParser::prepare() {
    return true;
}

void ThreadCdrParser::run() {
    try {
        FTPClient dataclient;
        ConfBill cb= app().conf;
        bool result = dataclient.ConnectToFTP(app().conf.cdr_ftp_host, app().conf.cdr_ftp_port, app().conf.cdr_ftp_user,
                                                  app().conf.cdr_ftp_password);
        if(result) {
//            result = dataclient.Login(app().conf.cdr_ftp_user, app().conf.cdr_ftp_password,
//                                      app().conf.cdr_ftp_responce_port);
            if(result) {
                std::string filecontent;
                std::list<std::string> filelist;
                int filescount = dataclient.GetLileList("/srv/ftp", filelist);
                BOOST_FOREACH(const std::string &filename, filelist) {
                    if (!isFileProcessed(filename)) {
                        dataclient.GetFile(filename, filecontent);
                        CdrParser parser(filecontent);
                        std::list<CallData> calls;
                        if (parser.Parse(calls)) {
                            BOOST_FOREACH(CallData calldata, calls) {
                                std::string insertcallquery;
                                insertcallquery = (boost::format(
                                    "select public.insert_cdr('%1%','%2%','%3%','%4%','%5%','%6%','%7%','%8%','%9%','%10%','%11%','%12%','%13%','%14%')")
                                           % calldata.call_id % app().conf.cdr_nasip %
                                           calldata.src_number %
                                           calldata.dst_number
                                           % calldata.redirect_number
                                           % calldata.session_time %
                                           calldata.setup_time %
                                           calldata.connect_time %
                                           calldata.disconnect_time
                                           % calldata.disconnect_cause %
                                           calldata.src_route % calldata.dst_route %
                                           calldata.src_noa
                                           % calldata.dst_noa).str();
                                BDbResult res = db_calls.query(insertcallquery);
                            }
                            std::string insertfilequery;
                            insertfilequery = (boost::format(
                                    "insert into calls_cdr.cdr_file(file_name, total_count, insert_count, error_count, error_ids)values('%1%', '%2%', '%3%', '0', '')")
                                               % filename % calls.size() % calls.size()).str();
                            BDbResult res = db_calls.query(insertfilequery);
                        }
                    }
                }
            }
            dataclient.Disconnect();
        }
    } catch(Exception &e) {
        int y = 0;
    }
}

bool ThreadCdrParser::isFileProcessed(const std::string &Filename) {
    std::string query = (boost::format("select file_name from calls_cdr.cdr_file where file_name='%1%'") % Filename).str();
    BDbResult res = db_calls.query(query);
    return (res.size() != 0);
};

//----------------------------------------------------------------------------------------------------------------------

CdrParser::CdrParser() {

}
CdrParser::CdrParser(const std::string &RawData) {
    rawdata = RawData;
}

bool CdrParser::Parse(CallDataList &Calls) {
    try {
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
            int callrecordposition = rawdata.find(callopentagname, beginpos + datalength);

            nextcallrecordposition = rawdata.find(callopentagname, callrecordposition + callopentagname.length());
            int callcloserecordposition = rawdata.find(callclosetagname, callrecordposition + callopentagname.length());

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
                return false;
            }
        }
    } catch (Exception &e) {
        return false;
    }
    return true;
}

bool CdrParser::Parse(const std::string &RawData, CallDataList &Calls) {
    rawdata = RawData;
    return Parse(Calls);
}

std::string CdrParser::ParseDateTime(const std::string &Data, bool CanBeEmpty)
{
    if(CanBeEmpty && (Data.size() == 0))
        return "";
    std::string result = (boost::format("%1%-%2%-%3% %4%:%5%:%6%.%7%+00") % Data.substr(0, 4) % Data.substr(4, 2) %
              Data.substr(6, 2)
              % Data.substr(8, 2) % Data.substr(10, 2) % Data.substr(12, 2) % Data.substr(15, 6)).str();
    return result;
}

void CdrParser::ParseNumber(const std::string &Data, std::string &Number, unsigned int &Noa, bool CanBeEmpty) {
    if((Data.size() > 0 ) || !CanBeEmpty) {
        int spacepos = Data.find(' ');
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
        int dividerpos = Data.find(':');
        if(std::string::npos != dividerpos) {
            callduration = boost::lexical_cast<unsigned long>(Data.substr(0, dividerpos)) + 1;
        } else {
            callduration = boost::lexical_cast<unsigned long>(Data) + 1;
        }
    }
}

int CdrParser::ParseDisconnectCause(const std::string &Data) {
    int resultcode(0);
    if(Data.size() >= 0) {
        int startpos = Data.find(':');
        if(std::string::npos != startpos) {
            int endpos = Data.find(' ', startpos + 1);
            std::string strresultcode = Data.substr(startpos + 1, endpos - startpos - 1);
            resultcode = boost::lexical_cast<int>(strresultcode);
        }
    }
    return resultcode;
}

bool CdrParser::ParseCall(const std::string &RawCallData, CallData &Call) {
    try {
        std::string value;
        Call.call_id = boost::lexical_cast<unsigned long>(GetValue("CallReference", RawCallData));
        value = GetValue("IncomingCallingAddress", RawCallData);
        ParseNumber(value, Call.src_number, Call.src_noa, true);
        value = GetValue("IncomingCalledAddress", RawCallData);
        ParseNumber(value, Call.dst_number, Call.dst_noa);
        value = GetValue("IncomingRedirectingAddress", RawCallData);
        unsigned int dummy;
        ParseNumber(value, Call.redirect_number, dummy, true);
        value = GetValue("StartTimeStamp", RawCallData);
        Call.setup_time = ParseDateTime(value);
        value = GetValue("AnswerTimeStamp", RawCallData);
        Call.connect_time = ParseDateTime(value, true);
        value = GetValue("ReleaseTimeStamp", RawCallData);
        Call.disconnect_time = ParseDateTime(value);
        value = GetValue("CallDuration", RawCallData);
        Call.session_time = ParseSessionTime(value);
        Call.src_route = GetValue("IncomingRouteId", RawCallData);
        Call.dst_route = GetValue("OutgoingRouteId", RawCallData);
        value = GetValue("CauseIndicator", RawCallData);
        Call.disconnect_cause = ParseDisconnectCause(value);
        Call.setup_time_raw = GetValue("StartTimeStamp", RawCallData);
    } catch(...) {
        return false;
    }
    return true;
}

std::string CdrParser::GetValue(const std::string &Name, const std::string &Data) {
    std::string value;
    try {
        std::string valuetofind = (boost::format("%1%>") % Name).str();
        std::size_t valuepos = Data.find(valuetofind);
        if(std::string::npos != valuepos) {
            valuepos += valuetofind.length();
            std::size_t endvaluepos = Data.find("<", valuepos);
            if(endvaluepos != std::string::npos) {
                value = Data.substr(valuepos, endvaluepos - valuepos);
            }
        }
    } catch(Exception &e) {
        value.empty();
    }
    return value;
}
