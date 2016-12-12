#include "FtpConnectStatus.h"

SystemStatus FtpConnectStatus::getStatus() {

    if (!app().threads.isRegistered("cdr_parser"))
        return healthStatus;

    try {

        string nextFileToParse;
        size_t not_processed = getFileNameForParse(nextFileToParse);
        healthStatus.statusId = HealthStatus::STATUS_OK;
        healthStatus.statusMessage = not_processed>0? "Next file to parse: "+nextFileToParse+" out of "+std::to_string(not_processed):"";
        return healthStatus;
    }
    catch (Exception &e) {

        healthStatus.statusId = HealthStatus::STATUS_ERROR;
        healthStatus.statusMessage = e.what();
        return healthStatus;
    }
}

int FtpConnectStatus::getFileNameForParse(string &lastNotParsedFileName) {

    std::vector<std::string> fileList;
    ftpClient.ls("/", fileList);

    files_total = fileList.size();
    files_processed = files_total;

    reverse(fileList.begin(), fileList.end());

    lastNotParsedFileName = "";
    for (std::string fileName : fileList) {

        if (isFileProcessed(fileName))
            break;

        lastNotParsedFileName = fileName;
        files_processed--;
    }
    return files_total-files_processed;
}

bool FtpConnectStatus::isFileProcessed(std::string &Filename) {

    std::string query = "select file_name from calls_cdr.cdr_file where file_name='"+Filename+"'";
    BDbResult res = db_calls.query(query);
    return (res.size() != 0);
};
