#include "ThreadCdrParser.h"
#include "../classes/CdrParser.h"
#include "../classes/AppBill.h"
#include <boost/format.hpp>
#include "../classes/FTPClient.h"
#include "../classes/DbException.h"
#include <regex>
#include <iostream>

ThreadCdrParser::ThreadCdrParser() : Thread()
{
    name ="CDR Parser";
    id = idName();
    threadSleepSeconds = app().conf.cdr_parcer_interval;
}

bool ThreadCdrParser::prepare() {
    return true;
}

void ThreadCdrParser::run() {
    if (app().conf.cdr_ftp_host == "") {
        return;
    }

    current_file_name = "";
    current_file_calls_count = 0;

    try {
        db_calls.setCS(app().conf.db_calls);
        FTPClient dataclient;

        bool result = dataclient.ConnectToFTP(app().conf.cdr_ftp_host, app().conf.cdr_ftp_user, app().conf.cdr_ftp_password);
        if (!result) {
            Log::error((boost::format("Can not connecting to CDR FTP server with parameters %1% %2% %3%") % app().conf.cdr_ftp_host % app().conf.cdr_ftp_user % app().conf.cdr_ftp_password).str());
            return;
        }

        std::string filecontent;
        std::list<std::string> filelist;
        unsigned long filescount = dataclient.GetLileList(app().conf.cdr_ftp_dir, filelist);

        files_on_server_count = filescount;

        for (auto filename : filelist) {
            if (isFileProcessed(filename)) {
                continue;
            }

            current_file_name = filename;

            result = dataclient.GetFile(filename, filecontent);
            if (!result) {
                throw Exception((boost::format("Error loading file content %1%") % filename).str(), "ThreadCdrParser::run");
            }

            CdrParser parser(filecontent);
            std::list<CallData> calls;
            if (!parser.Parse(calls)) {
                throw Exception((boost::format("Can not parse file %1%") % filename).str(), "ThreadCdrParser::run");
            }

            current_file_calls_count = calls.size();

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
                db_calls.query(insertcallquery);
                processed_calls_count++;
            }

            CdrFile file;
            file.file_name = filename;
            file.total_count = calls.size();
            file.insert_count = calls.size() - parser.GetErrorCallsCount();
            file.error_count = parser.GetErrorCallsCount();
            file.error_ids = parser.GetErrorIDs();

            std::string insertfilequery;
            insertfilequery = (boost::format(
                    "insert into calls_cdr.cdr_file(file_name, total_count, insert_count, error_count, error_ids)values('%1%', '%2%', '%3%', '%4%', '%5%')")
                               % file.file_name % file.total_count % file.insert_count % file.error_count % file.error_ids).str();
            db_calls.exec(insertfilequery);

            last_files.push_back(file);

            processed_files_count++;


            if (parser.GetErrorCallsCount() > 0) {
                Log::error((boost::format("File %1% has %2% errors. ID %3%") % filename %
                            (calls.size() - parser.GetErrorCallsCount()) % parser.GetErrorIDs()).str());
            }

        }
    } catch(Exception &e) {
        e.addTrace("CDR parsing error.");
        throw e;
    }


    current_file_name = "";
    current_file_calls_count = 0;
}

bool ThreadCdrParser::isFileProcessed(const std::string &Filename) {
    try {
        std::string query = (boost::format("select file_name from calls_cdr.cdr_file where file_name='%1%'") %
                             Filename).str();
        BDbResult res = db_calls.query(query);
        return (res.size() != 0);
    } catch (Exception &e) {
        std::string message((boost::format("File %1% status error") % Filename).str());
        Log::error(message);
        return true;
    }
};

bool ThreadCdrParser::hasFullHtml() {
    return true;
}

void ThreadCdrParser::htmlfull(stringstream & html) {
    this->html(html);

    html << "Files on server count: <b>" << files_on_server_count << "</b><br/>\n";
    html << "Processed files: <b>" << processed_files_count << "</b><br/>\n";
    html << "Processed calls count: <b>" << processed_calls_count << "</b><br/>\n";
    html << "Current file name: <b>" << current_file_name << "</b><br/>\n";
    html << "Current file calls count: <b>" << current_file_calls_count << "</b><br/>\n";

    html << "<br/>\n";

    html << "Last uploaded files\n";
    html << "<table>\n";
    html << "<tr>\n";
    html << "<th>File name</th>\n";
    html << "<th>total</th>\n";
    html << "<th>inserted</th>\n";
    html << "<th>errors</th>\n";
    html << "<th>errors ids</th>\n";
    html << "</tr>\n";
    for (CdrFile &file : last_files) {
        html << "<tr>\n";
        html << "<td>" << file.file_name << "</td>\n";
        html << "<td>" << file.total_count << "</td>\n";
        html << "<td>" << file.insert_count << "</td>\n";
        html << "<td>" << file.error_count << "</td>\n";
        html << "<td>" << file.error_ids << "</td>\n";
        html << "</tr>\n";
    }
    html << "</table>\n";

}
