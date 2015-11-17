#pragma once

#include <boost/array.hpp>
#include <list>
#include <boost/asio.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/write.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>

class FTPClient {
private:
    boost::asio::ip::tcp::socket *ftpsocket;
    boost::asio::io_service ftpservice;
    boost::asio::ip::tcp::socket *receiversocket;
    boost::asio::io_service receiverservice;
    std::map<int, std::string> lastresponces;
public:
    FTPClient();
    bool ConnectToFTP(const std::string &Server, const std::string &User, const std::string &Password);
    bool Disconnect();

    unsigned long GetLileList(const std::string &Path, std::list<std::string> &FileList);
    bool GetFile(const std::string &Filename, std::string &FileContent);

    void ExecuteCommand(const std::string &Command, int DesiredResponceCode);
    void ExecuteCommand(const std::string &Command, std::string &Responce, int DesiredResponceCode, unsigned long DataSize);
    bool IsResponceCode(int ResponceCode);
    std::string GetResponceData(int ResponceCode);
private:
    bool EnterPassiveMode(const std::string &Host, unsigned short Port);
    void LeavePassiveMode();
    unsigned short ParseForPort(const std::string &Message, std::string &Host);
    void ReceiveResponce(int DesiredResponceCode);
    void ReceiveRaw(std::string &Responce, unsigned long DataSize);
};
