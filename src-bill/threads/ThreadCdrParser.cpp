#include "ThreadCdrParser.h"
#include "../classes/CdrParser.h"
#include "../classes/AppBill.h"
#include <boost/format.hpp>
#include "../classes/FTPClient.h"
#include "../classes/LogWriterSyslog.h"
#include <regex>
#include <iostream>

ThreadCdrParser::ThreadCdrParser() : Thread()
{
    name ="CDR Parser";
    id = idName();
    threadSleepSeconds = app().conf.cdr_parcer_timeout;
}

bool ThreadCdrParser::prepare() {
    return true;
}

void ThreadCdrParser::run() {
    try {
        db_calls.setCS(app().conf.db_calls);
        Log::info("Start CDR parsing process...");
        FTPClient dataclient;
        Log::info((boost::format("Connecting to CDR FTP server with parameters %1% %2% %3%") % app().conf.cdr_ftp_host % app().conf.cdr_ftp_user % app().conf.cdr_ftp_password).str());
        bool result = dataclient.ConnectToFTP(app().conf.cdr_ftp_host, app().conf.cdr_ftp_user, app().conf.cdr_ftp_password);
        if(result) {
            Log::info("OK");
            std::string filecontent;
            std::list<std::string> filelist;
            Log::info("Loading CDR files list...");
            unsigned long filescount = dataclient.GetLileList(app().conf.cdr_ftp_dir, filelist);
            Log::info((boost::format("%1% files found") % filescount).str());
            for (auto filename : filelist) {
                Log::info((boost::format("Processing %1%") % filename).str());
                if (!isFileProcessed(filename)) {
                    Log::info("Loading file content...");
                    result = dataclient.GetFile(filename, filecontent);
                    if(result) {
                        CdrParser parser(filecontent);
                        std::list<CallData> calls;
                        if (parser.Parse(calls)) {
                            for (auto calldata : calls) {
                                std::string insertcallquery;
                                insertcallquery = (boost::format(
                                        "select public.insert_cdr('%1%','%2%','%3%','%4%','%5%','%6%','%7%','%8%','%9%','%10%','%11%','%12%','%13%','%14%')")
                                                   % calldata.call_id % app().conf.cdr_nasip
                                                   % calldata.src_number % calldata.dst_number
                                                   % calldata.redirect_number % calldata.session_time
                                                   % calldata.setup_time % calldata.connect_time
                                                   % calldata.disconnect_time % calldata.disconnect_cause
                                                   % calldata.src_route % calldata.dst_route
                                                   % calldata.src_noa % calldata.dst_noa).str();
                                BDbResult res = db_calls.query(insertcallquery);
                                if (res.next()) {
                                    LogWriterSyslog syslog((boost::format("File %1% CDR with id %2% database writing error") % filename % calldata.call_id).str(), LogLevel::ERROR);
                                }
                            }
                            std::string insertfilequery;
                            insertfilequery = (boost::format(
                                    "insert into calls_cdr.cdr_file(file_name, total_count, insert_count, error_count, error_ids)values('%1%', '%2%', '%3%', '%4%', '%5%')")
                                               % filename % calls.size() %
                                               (calls.size() - parser.GetErrorCallsCount()) %
                                               parser.GetErrorCallsCount() % parser.GetErrorIDs()).str();
                            if(parser.GetErrorCallsCount() > 0) {
                                LogWriterSyslog syslog((boost::format("File %1% has %2% errors. ID %3%") % filename % (calls.size() - parser.GetErrorCallsCount()) % parser.GetErrorIDs()).str(), LogLevel::ERROR);
                            }
                            try {
                                BDbResult res = db_calls.query(insertfilequery);
                                if (res.next()) {
                                    LogWriterSyslog syslog((boost::format("File %1% database writing error") % filename).str(), LogLevel::ERROR);
                                }
                            } catch(Exception &e) {
                                LogWriterSyslog syslog((boost::format("File %1% database writing error. Reason: %2%") % filename % e.what()).str(), LogLevel::ERROR);
                            }
                        }
                    } else {
                        Log::error((boost::format("%1% loading error") % filename).str());
                    }
                } else {
                    Log::info("File processed");
                }
            }
            dataclient.Disconnect();
        } else {
            Log::error("CDR FTP connection error");
        }
        db_calls.disconnect();
    } catch(Exception &e) {
        Log::error((boost::format("CDR parsing error. Reason: %1%") % e.what()).str());
    } catch(...) {
        LogWriterSyslog syslog("CDR parsing general error", LogLevel::ERROR);
        Log::error("CDR parsing general error");
    }
    Log::info("End of CDR parsing process...");
}

bool ThreadCdrParser::isFileProcessed(const std::string &Filename) {
    try {
        std::string query = (boost::format("select file_name from calls_cdr.cdr_file where file_name='%1%'") %
                             Filename).str();
        BDbResult res = db_calls.query(query);
        return (res.size() != 0);
    } catch (Exception &e) {
        std::string message((boost::format("File %1% status error") % Filename).str());
        LogWriterSyslog syslog(message, LogLevel::ERROR);
        Log::error(message);
        return true;
    }
};
