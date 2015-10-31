//#include "../classes/AppBill.h"
#include "Exception.h"
#include <string>
#include <iostream>
#include <ostream>
#include <boost/bind.hpp>
#include <boost/make_shared.hpp>
#include <boost/format.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/lexical_cast.hpp>
#include "FTPClient.h"
#include <regex>

using namespace boost::asio;
using namespace boost::asio::ip;

FTPClientReceiver::FTPClientReceiver(boost::asio::io_service& service, unsigned short port)
        : strand_(service)
        , acceptor_(service, tcp::endpoint(tcp::v4(), port))
        , socket_(service)
        , index_(0) {
}

FTPClientReceiver::time_stamps& FTPClientReceiver::Stamps()
{
    return stamps_;
}

void FTPClientReceiver::Start()
{
    buffer_.fill(0);
    datafromserver.clear();
    acceptor_.async_accept(socket_, boost::bind(&FTPClientReceiver::HandleAccept, this, boost::asio::placeholders::error));
}

void FTPClientReceiver::Stop() {
    acceptor_.close();
    socket_.close();
}

void FTPClientReceiver::HandleAccept(const boost::system::error_code& error)
{
//    datafromserver.clear();
    if(error)
    {
        std::cout << error.message() << std::endl;
        return;
    }
//    read_header();
    Read();
}
/*
void FTPClientReceiver::ReadHeader()
{
    boost::asio::async_read(
            socket_,
            boost::asio::buffer(buffer_, header_size),
            boost::bind( &FTPClientReceiver::HandleReadHeader, this,
                         boost::asio::placeholders::error,
                         boost::asio::placeholders::bytes_transferred));
}
*/
void FTPClientReceiver::Read()
{
    buffer_.fill(0);
    boost::asio::async_read(
            socket_,
            boost::asio::buffer(buffer_, buffer_size - 1),
            boost::bind( &FTPClientReceiver::HandleReadData, this,
                         boost::asio::placeholders::error,
                         boost::asio::placeholders::bytes_transferred));
}
/*
void  FTPClientReceiver::HandleReadHeader(const boost::system::error_code &error, std::size_t bytes_transferred)
{
    if(error)
    {
        std::cout << error.message() << std::endl;
        return;
    }
    if(!RecordStamp()) {
        socket_.get_io_service().stop();
        return;
    }
    boost::asio::async_read(
            socket_,
            boost::asio::buffer(buffer_, data_size),
            boost::bind( &FTPClientReceiver::HandleReadData, this,
                         boost::asio::placeholders::error,
                         boost::asio::placeholders::bytes_transferred));

}
*/
void FTPClientReceiver::HandleReadData(const boost::system::error_code &error, std::size_t bytes_transferred)
{
    int ec = error.value();
    switch(ec) {
        case boost::asio::error::eof:
            if(bytes_transferred > 0) {
                datafromserver += buffer_.c_array();
            }
            std::cout << "FTP Data received. Size " << datafromserver.length() << "bytes." << std::endl;
            socket_.get_io_service().stop();
            break;
        case boost::system::errc::success:
            datafromserver += buffer_.c_array();
            if(!RecordStamp()) {
                socket_.get_io_service().stop();
                return;
            }
            break;
        default:
            std::cout << error.message() << std::endl;
            socket_.get_io_service().stop();
            return;
    }
//    read_header();
    Read();
}


bool FTPClientReceiver::RecordStamp()
{
    stamps_[ index_++ ] = boost::posix_time::microsec_clock::local_time();
    return index_ < max_stamp;
}

std::string FTPClientReceiver::GetDataFromServer() {
    return datafromserver;
}
//----------------------------------------------------------------------------------------

FTPClient::FTPClient()
{
}

bool FTPClient::ConnectToFTP(const std::string &Server, int Port, const std::string &User, const std::string &Password) {
    try {
        tcp::resolver ftpresolver(ftpservice);
        tcp::resolver::query query(Server, "ftp");
        tcp::resolver::iterator endpoint_iterator = ftpresolver.resolve(query);
        tcp::resolver::iterator end;
        ftpsocket = new tcp::socket(ftpservice);
        boost::system::error_code error = boost::asio::error::host_not_found;
        while (error && endpoint_iterator != end)
        {
            ftpsocket->close();
            ftpsocket->open(ip::tcp::v4());
            ftpsocket->connect(*endpoint_iterator++, error);
        }
        if (!error) {
            std::string usercommand;
            usercommand = (boost::format("USER %1%") % User).str();
            std::string passwordncommand;
            passwordncommand = (boost::format("PASS %1%") % Password).str();
            std::string result;
            ExecuteCommand(usercommand);
            if(IsResponceCode(331) || IsResponceCode(220)) {
                ExecuteCommand(passwordncommand);
                if(IsResponceCode(230) || IsResponceCode(331)) {
                    ExecuteCommand("PASV");
                    if(IsResponceCode(127) || IsResponceCode(230)) {
                        return true;
                    }
                }
            }
        }
    } catch (Exception &e) {
        e.addTrace("ThreadCdrParser: Can't open FTP connection. Reason: " + e.message);
    }
    return false;
}

bool FTPClient::Disconnect() {
    ftpsocket->close();
}

void FTPClient::ReceiveResponce(boost::asio::ip::tcp::socket *Receiver) {
    boost::asio::streambuf responsebuffer;
    boost::asio::read_until(*Receiver, responsebuffer, "\r\n");
    std::istream response_stream(&responsebuffer);
    while (!response_stream.eof()) {
        std::string responce;
        std::getline(response_stream, responce);
        boost::replace_all(responce, "\r", "");
        if(responce.length() > 3) {
            int responcesode = boost::lexical_cast<int>(responce.substr(0, 3));
            responce.erase(0, 3);
            lastresponces[responcesode] = responce;
        }
    }
}

std::string FTPClient::ReceiveRaw(boost::asio::ip::tcp::socket *Receiver) {
    std::string responce;
    boost::asio::streambuf responsebuffer;
    boost::asio::read_until(*Receiver, responsebuffer, "\0");
    std::istream response_stream(&responsebuffer);
    while (!response_stream.eof()) {
        std::string tmp;
        response_stream >> tmp;
        responce += tmp;
    }
    return responce;
}

bool FTPClient::IsResponceCode(int ResponceCode) {
    return (lastresponces.find(ResponceCode) != lastresponces.end());
}

std::string FTPClient::GetResponceData(int ResponceCode) {
    std::map<int, std::string>::const_iterator responce;
    responce = lastresponces.find(ResponceCode);
    return (responce != lastresponces.end()) ? responce->second : "" ;
}

void FTPClient::ExecuteCommand(const std::string &Command) {
    std::string buffer;
    buffer = Command + "\r\n";
    boost::asio::write(*ftpsocket, boost::asio::buffer(buffer, buffer.length()));
    ReceiveResponce(ftpsocket);
}

void FTPClient::ExecuteCommand(const std::string &Command, std::string &Responce) {
    std::string buffer;
    buffer = Command + "\r\n";
    boost::asio::write(*ftpsocket, boost::asio::buffer(buffer, buffer.length()));
    Responce = ReceiveRaw(receiversocket);
    ReceiveResponce(ftpsocket);
}
/*
bool FTPClient::Login(const std::string &User, const std::string &Password, int ResponcePort) {
    responeceport = ResponcePort;
    std::string usercommand;
    usercommand = (boost::format("USER %1%") % User).str();
    std::string passwordncommand;
    passwordncommand = (boost::format("PASS %1%") % Password).str();
    std::string result;
    ExecuteCommand(usercommand);
    if(lasterrorcode == 331) {
        ExecuteCommand(passwordncommand);
        return (lasterrorcode == 220) ? true : false;
    } else {
        return false;
    }
}
*/

bool FTPClient::EnterPassiveMode(const std::string &Host, unsigned short Port) {
    tcp::resolver ftpreceiver(receiverservice);
    tcp::resolver::query receiverquery(Host, boost::lexical_cast<std::string>(Port));
    tcp::resolver::iterator endpoint_iterator = ftpreceiver.resolve(receiverquery);
    tcp::resolver::iterator end;
    receiversocket = new tcp::socket(receiverservice);
    boost::system::error_code error = boost::asio::error::host_not_found;
    while (error && endpoint_iterator != end)
    {
        receiversocket->close();
        receiversocket->open(ip::tcp::v4());
        receiversocket->connect(*endpoint_iterator++, error);
    }
    return (error == boost::system::errc::success);
}

int FTPClient::GetLileList(const std::string &Path, std::list<std::string> &FileList) {
    try {
//        std::string command = (boost::format("CWD %1%") % Path).str();
//        ExecuteCommand(command);
//        if(IsResponceCode(250)) {
            std::string host;
            std::string connectdata = GetResponceData(227);
            if(!connectdata.empty()) {
                unsigned short port = ParseForPort(connectdata, host);
                if (EnterPassiveMode(host, port)) {
                    std::string fileleist;
                    ExecuteCommand("NLST", fileleist);
                    if (IsResponceCode(250)) {
                        boost::replace_all(fileleist, "\r", "");
                        boost::split(FileList, fileleist, boost::is_any_of("\n"));
                        return FileList.size();
                    }
                }
            }
           return 0;
//        }
    } catch(Exception &e) {
        return 0;
    }
}

bool FTPClient::GetFile(const std::string &Filename, std::string &FileContent) {
    ExecuteCommand("TYPE A");
    if(IsResponceCode(226)) {
        ExecuteCommand("STRU F");
        if(IsResponceCode(200)) {
            boost::asio::io_service service;
            std::string command = (boost::format("RETR %1%") % Filename).str();
            currentfilename = Filename;
            ExecuteCommand(command, FileContent);
            return true;
        }
    }
    return false;
}

bool FTPClient::ExecuteCommandWithResponce(const std::string &Command, std::string &Responce) {

//    std::string portcommand = "PASV";
//    std::string portcommand = (boost::format("PORT 10,0,2,15,%1%,%2%") % (responeceport / 256) % (responeceport % 256)).str();
//    ExecuteCommand(portcommand);
//    std::string host;
//    unsigned short port = ParseForPort(lastmessage, host);

//    boost::asio::io_service service;
//    boost::shared_ptr<FTPClientReceiver> receiver = boost::make_shared<FTPClientReceiver>(boost::ref(service), port);
//    receiver->Start();
//    if(lasterrorcode == 227) {
//        ExecuteCommand(Command);
//        if(lasterrorcode == 150) {
//            service.run();
//            receiver->Stop();
//            Responce = receiver->GetDataFromServer();
//            return true;
//        }
//    }
    return false;
}

unsigned short FTPClient::ParseForPort(const std::string &Message, std::string &Host) {
    std::regex re("([0-9]*),([0-9]*),([0-9]*),([0-9]*),([0-9]*),([0-9]*)");
    std::smatch match;
    if (std::regex_search(Message, match, re) && (match.size() >= 7)) {
        int first = boost::lexical_cast<int>(match[5]);
        int second = boost::lexical_cast<int>(match[6]);
        int hostfirst = boost::lexical_cast<int>(match[1]);
        int hostsecond = boost::lexical_cast<int>(match[2]);
        int hostthird = boost::lexical_cast<int>(match[3]);
        int hostfourth = boost::lexical_cast<int>(match[4]);
        Host = (boost::format("%1%.%2%.%3%.%4%") % hostfirst % hostsecond % hostthird % hostfourth).str();
        return (first << 8) + second;
    } else {
        return 21;
    }
}