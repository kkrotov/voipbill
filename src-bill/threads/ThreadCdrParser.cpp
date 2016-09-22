#include "ThreadCdrParser.h"
#include "../classes/AppBill.h"
#include <boost/format.hpp>
#include <regex>
#include <iostream>

ThreadCdrParser::ThreadCdrParser() : Thread()
{
    name ="CDR Parser";
    id = idName();
    threadSleepSeconds = app().conf.cdr_parcer_interval;

    ftpClient.setHost(app().conf.cdr_ftp_host);
    ftpClient.setUserName(app().conf.cdr_ftp_user);
    ftpClient.setPassword(app().conf.cdr_ftp_password);

    db_calls.setCS(app().conf.db_calls);
}

void ThreadCdrParser::run() {
    parser_log = "";
    cdrFile.clear();

    if (app().conf.cdr_ftp_host == "") {
        return;
    }

    try {

        string fileName = getFileNameForParse();
        while (fileName.size() > 0) {

            processFile(fileName);

            fileName = getFileNameForParse();

        }

    } catch(Exception &e) {
        logParser("parser error: " + e.message);
        e.addTrace("CDR parsing error.");
        throw e;
    }

    cdrFile.clear();
}

void ThreadCdrParser::processFile(const string &fileName) {

    cdrFile.clear();
    cdrFile.file_name = fileName;

    string fileContent;
    fetchFile(fileContent);

    std::list<CallData> calls;
    if (parseFile(calls, fileContent))
        saveCalls(calls);

    saveCdrFile();
}

void ThreadCdrParser::fetchFile(string &fileContent) {
    logParser("getting file " + cdrFile.file_name);

    ftpClient.fetch("/" + cdrFile.file_name, fileContent);
}

bool ThreadCdrParser::parseFile(std::list<CallData> &calls, const string &fileContent) {
    logParser("parsing file " + cdrFile.file_name);

    if (!parser.Parse(calls, fileContent)) {
        Log::error((boost::format("Can not parse file %1%") % cdrFile.file_name).str());
        return false;
    }

    if (parser.GetErrorCallsCount() > 0) {
        Log::error((boost::format("File %1% has %2% errors. ID %3%") % cdrFile.file_name %
                    (calls.size() - parser.GetErrorCallsCount()) % parser.GetErrorIDs()).str());
        return false;
    }

    cdrFile.total_count = calls.size();
    cdrFile.error_count = parser.GetErrorCallsCount();
    cdrFile.error_ids = parser.GetErrorIDs();
    return true;
}

void ThreadCdrParser::saveCalls(const std::list<CallData> &calls) {
    logParser("saving calls " + cdrFile.file_name);

    for (auto call : calls) {
        cdrFile.progress++;

        if (!call.IsFinished() && !isUnfinishedCallExists(call.hash, call.dst_route)) {

            saveUnfinishedCall(call);
            logUnfinishedCall(call);
            continue;
        }
        saveCall(call);
    }
}

void ThreadCdrParser::saveUnfinishedCall(const CallData &call) {

    try {

        std::string query = (boost::format("select public.insert_cdr_unfinished('%1%','%2%','%3%','%4%','%5%','%6%','%7%','%8%','%9%')")
                             % call.call_id % call.setup_time %call.src_number %call.dst_number %call.src_route % call.dst_route
                             % call.releasing_party % call.release_timestamp % call.disconnect_cause).str();
        db_calls.query(query);
    }
    catch (Exception &e) {

        std::string message = "Error adding new unfinished call "+call.call_id+": "+e.message;
        Log::error(message);
    }
}

void ThreadCdrParser::logUnfinishedCall(CallData &call) {

    pLogMessage logCall(new LogMessage());

    logCall->type = "call";
    logCall->params["call_id"] = call.call_id;
    logCall->params["src"] = call.src_number;
    logCall->params["dst"] = call.dst_number;
    logCall->params["dst_replace"] = call.dst_replace;
    logCall->params["redirect_number"] = call.redirect_number;

    logCall->params["src_route"] = call.src_route;
    logCall->params["dst_route"] = call.dst_route;

    logCall->params["src_noa"] = call.src_noa;
    logCall->params["dst_noa"] = call.dst_noa;

    logCall->params["disconnect_cause"] = call.disconnect_cause;
    logCall->params["call_finished"] = "No";

    Log::info(logCall); // Call "+call.call_id+" is UNFINISHED
}


void ThreadCdrParser::saveCall(const CallData &call) {
    std::string query = (boost::format(
            "select public.insert_cdr2('%1%','%2%','%3%','%4%','%5%','%6%','%7%','%8%','%9%','%10%','%11%','%12%','%13%','%14%','%15%','%16%','%17%')")
                       % call.call_id % app().conf.cdr_nasip
                       % call.src_number % call.dst_number
                       % call.redirect_number % call.session_time
                       % call.setup_time % call.connect_time
                       % call.disconnect_time % call.disconnect_cause
                       % call.src_route % call.dst_route
                       % call.src_noa % call.dst_noa
                       % call.dst_replace
                       % call.call_finished % call.releasing_party).str();
    db_calls.query(query);

    cdrFile.insert_count++;
}

bool ThreadCdrParser::isCallExists(const string &hash) {
    BDbResult res = db_calls.query("select hash from calls_cdr.cdr where hash='" + hash + "' limit 1");
    return (res.next() && res.get_s(0).size() > 0);
}

bool ThreadCdrParser::isUnfinishedCallExists(const string &hash, string dst_route) {
    BDbResult res = db_calls.query("select hash from calls_cdr.cdr_unfinished where hash='" + hash + "' and dst_route='"+dst_route+"' limit 1");
    return (res.next() && res.get_s(0).size() > 0);
}

void ThreadCdrParser::saveCdrFile() {
    logParser("saving cdr file " + cdrFile.file_name);

    std::string query = (boost::format(
            "insert into calls_cdr.cdr_file(file_name, total_count, insert_count, error_count, error_ids)values('%1%', '%2%', '%3%', '%4%', '%5%')")
                       % cdrFile.file_name % cdrFile.total_count % cdrFile.insert_count % cdrFile.error_count % cdrFile.error_ids).str();
    db_calls.exec(query);

    last_files.push_back(cdrFile);
}

string ThreadCdrParser::getFileNameForParse() {
    logParser("getting files list");

    std::vector<std::string> fileList;
    ftpClient.ls("/", fileList);

    logParser(lexical_cast<string>(fileList.size()) + " files found");

    files_total = fileList.size();
    files_progres = files_total;

    reverse(fileList.begin(), fileList.end());

    string lastNotParsedFileName = "";
    for (auto fileName : fileList) {

        if (isFileProcessed(fileName)) {
            break;
        }

        lastNotParsedFileName = fileName;
        files_progres--;
    }

    return lastNotParsedFileName;
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

void ThreadCdrParser::logParser(const std::string &message) {
    parser_log += string_time(time(nullptr)) + " " + message + "\n";
}

bool ThreadCdrParser::hasFullHtml() {
    return true;
}

void ThreadCdrParser::htmlfull(stringstream & html) {
    this->html(html);

    html << "Files total: <b>" << files_total << "</b><br/>\n";
    html << "Files progress: <b>" << files_progres << "</b><br/>\n";
    html << "Current file name: <b>" << cdrFile.file_name << "</b><br/>\n";
    html << "Current file total count: <b>" << cdrFile.total_count << "</b><br/>\n";
    html << "Current file inserted count: <b>" << cdrFile.insert_count << "</b><br/>\n";
    html << "Current file error count: <b>" << cdrFile.error_count << "</b><br/>\n";
    html << "Current progress: <b>" << cdrFile.progress << "</b><br/>\n";

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

    html << "<br/>\n";
    html << "Parser log:<br/>\n";
    string log = parser_log;
    replace_all(log, "\n", "<br/>\n");
    html << log;

}

