#pragma once

#include <string>
#include <vector>

class FtpClient {
private:
    std::string host;
    std::string username;
    std::string password;
public:
    FtpClient();
    FtpClient(std::string host, std::string username, std::string password);
    void setHost(std::string host);
    void setUserName(std::string username);
    void setPassword(std::string password);

    void ls(std::string uri, std::vector<std::string> &files);
    void fetch(std::string uri, std::string &data);
};
