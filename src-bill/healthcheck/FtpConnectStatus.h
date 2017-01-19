#pragma once

#include "HealthCheckController.h"
#include "../classes/FtpClient.h"
#include "../classes/AppBill.h"
#include "../classes/BDb.h"

class FtpConnectStatus : public HealthCheck {

    FtpClient ftpClient;
    BDb db_calls;
    size_t files_total, files_processed;

    int getFileNameForParse(string &lastNotParsedFileName);
    bool isFileProcessed(std::string &Filename);

public:
    FtpConnectStatus() : HealthCheck("FtpConnect") {

        ftpClient.setHost(app().conf.cdr_ftp_host);
        ftpClient.setUserName(app().conf.cdr_ftp_user);
        ftpClient.setPassword(app().conf.cdr_ftp_password);

        db_calls.setCS(app().conf.db_calls);
        healthStatus.statusId = HealthStatus::STATUS_UNKNOWN;
    }
    SystemStatus getStatus() override;
};
