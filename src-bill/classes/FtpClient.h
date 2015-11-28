#pragma once

#include <string>
#include <bits/stl_list.h>

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

    void ls(std::string uri, std::list<std::string> &files);
    void fetch(std::string uri, std::string &data);
};
