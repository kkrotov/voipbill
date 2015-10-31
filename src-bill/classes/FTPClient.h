#pragma once

#include <boost/array.hpp>
#include <list>
#include <boost/asio.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/write.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>

class FTPClientReceiver : public boost::enable_shared_from_this<FTPClientReceiver>
{
private:
    enum
    {
//        header_size = 4,
//        data_size   = 10,
        buffer_size = 1024,
        max_stamp   = 10000
    };
public:
    typedef boost::array<boost::posix_time::ptime, max_stamp> time_stamps;
public:
    FTPClientReceiver(boost::asio::io_service& service, unsigned short port);
    void Start();
    void Stop();

    std::string GetDataFromServer();
    time_stamps& Stamps();
private:
    void HandleAccept(const boost::system::error_code& error);
    void ReadHeader();
    void HandleReadHeader(const boost::system::error_code &error, std::size_t bytes_transferred);
    void HandleReadData(const boost::system::error_code &error, std::size_t bytes_transferred);

    bool RecordStamp();
    void Read();
private:
    std::string datafromserver;
    boost::asio::io_service::strand strand_;
    boost::asio::ip::tcp::acceptor acceptor_;
    boost::asio::ip::tcp::socket socket_;
    boost::array<char, buffer_size> buffer_;
    time_stamps stamps_;
    unsigned int index_;
};

class FTPClient {
private:
    boost::asio::ip::tcp::socket *ftpsocket;
    boost::asio::io_service ftpservice;
    boost::asio::ip::tcp::socket *receiversocket;
    boost::asio::io_service receiverservice;
    std::map<int, std::string> lastresponces;
    std::string currentfilename;
    int responeceport;
public:
    FTPClient();
    bool ConnectToFTP(const std::string &Server, int Port, const std::string &User, const std::string &Password);
//    bool Login(const std::string &User, const std::string &Password, int ResponcePort);
    bool Disconnect();

    int GetLileList(const std::string &Path, std::list<std::string> &FileList);
    bool GetFile(const std::string &Filename, std::string &FileContent);

    void ExecuteCommand(const std::string &Command);
    void ExecuteCommand(const std::string &Command, std::string &Responce);
    bool IsResponceCode(int ResponceCode);
    std::string GetResponceData(int ResponceCode);
private:
    bool EnterPassiveMode(const std::string &Host, unsigned short Port);
    unsigned short ParseForPort(const std::string &Message, std::string &Host);
    void ReceiveResponce(boost::asio::ip::tcp::socket *Receiver);
    std::string ReceiveRaw(boost::asio::ip::tcp::socket *Receiver);
    bool ExecuteCommandWithResponce(const std::string &Command, std::string &Responce);
};
