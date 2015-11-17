#include "Exception.h"
//#include <string>
#include <iostream>
//#include <ostream>
#include <boost/bind.hpp>
#include <boost/make_shared.hpp>
#include <boost/format.hpp>
#include <boost/algorithm/string.hpp>
//#include <boost/algorithm/string/split.hpp>
#include <boost/lexical_cast.hpp>
#include "FTPClient.h"
#include <regex>
//#include <unistd.h>
//#include <boost/thread.hpp>
#include <thread>

using namespace boost::asio;
using namespace boost::asio::ip;

FTPClient::FTPClient()
{
}

bool FTPClient::ConnectToFTP(const std::string &Server, const std::string &User, const std::string &Password) {
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
            ExecuteCommand(usercommand, 331);
            if(IsResponceCode(331)/* || IsResponceCode(220)*/) {
                ExecuteCommand(passwordncommand, 230);
                if(IsResponceCode(230)) {
                    return true;
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
    return true;
}

void FTPClient::ReceiveResponce(int DesiredResponceCode) {
//    usleep(50000);
//    sleep(1);
    lastresponces.clear();
    bool codefound(false);
    int attemptcout(100 * 30); // 30 seconds
    do {
        usleep(10000);
        std::string responce;
        size_t avail = ftpsocket->available();
        while (avail > 0){
            boost::asio::streambuf responsebuffer;
            boost::asio::read_until(*ftpsocket, responsebuffer, "\0");
            boost::asio::streambuf::const_buffers_type bufs = responsebuffer.data();
            std::string tmp(boost::asio::buffers_begin(bufs), boost::asio::buffers_begin(bufs) + responsebuffer.size());
            responce += tmp;
            avail = ftpsocket->available();
        };
        boost::replace_all(responce, "\r", "");
        std::list<std::string> tmp;
        boost::split(tmp, responce, boost::is_any_of("\n"));
        for (auto message : tmp) {
            if (message.length() > 3) {
                int responcesode = boost::lexical_cast<int>(message.substr(0, 3));
                message.erase(0, 3);
                lastresponces[responcesode] = message;
                if (responcesode == DesiredResponceCode)
                    codefound = true;
                std::cout << responcesode << message << std::endl;
            }
        }
    } while(!codefound && (--attemptcout > 0));
/*
    boost::asio::streambuf responsebuffer;
    boost::asio::read_until(*Receiver, responsebuffer, "\0");
    std::istream response_stream(&responsebuffer);
    while (!response_stream.eof()) {
        std::string responce;
        std::getline(response_stream, responce);
        boost::replace_all(responce, "\r", "");
        if(responce.length() > 3) {
            int responcesode = boost::lexical_cast<int>(responce.substr(0, 3));
            responce.erase(0, 3);
            lastresponces[responcesode] = responce;
            std::cout << responcesode << responce << std::endl;
        }
    }*/
    std::cout << std::endl;
}

void FTPClient::ReceiveRaw(std::string &Responce, unsigned long DataSize) {
    std::string responce;
    if(DataSize == 0) {
        size_t avail = receiversocket->available();
        while (avail > 0) {
            boost::asio::streambuf responsebuffer;
            boost::asio::read_until(*receiversocket, responsebuffer, "\0");
            boost::asio::streambuf::const_buffers_type bufs = responsebuffer.data();
            std::string tmp(boost::asio::buffers_begin(bufs), boost::asio::buffers_begin(bufs) + responsebuffer.size());
            responce += tmp;
            avail = receiversocket->available();
        }
    } else {
        while (DataSize != responce.length()) {
            size_t avail = receiversocket->available();
            if(avail > 0) {
                boost::asio::streambuf responsebuffer;
                boost::asio::read_until(*receiversocket, responsebuffer, "\0");
                boost::asio::streambuf::const_buffers_type bufs = responsebuffer.data();
                std::string tmp(boost::asio::buffers_begin(bufs),
                                boost::asio::buffers_begin(bufs) + responsebuffer.size());
                responce += tmp;
            }
            //        avail = receiversocket->available();
        }
    }
    Responce = responce;
}

bool FTPClient::IsResponceCode(int ResponceCode) {
    return (lastresponces.end() != lastresponces.find(ResponceCode));
}

std::string FTPClient::GetResponceData(int ResponceCode) {
    std::map<int, std::string>::const_iterator responce;
    responce = lastresponces.find(ResponceCode);
    return (responce != lastresponces.end()) ? responce->second : "" ;
}

void FTPClient::ExecuteCommand(const std::string &Command, int DesiredResponceCode) {
    std::string buffer;
    buffer = Command + "\r\n";
    boost::asio::write(*ftpsocket, boost::asio::buffer(buffer, buffer.length()));
    ReceiveResponce(DesiredResponceCode);
}

void FTPClient::ExecuteCommand(const std::string &Command, std::string &Responce, int DesiredResponceCode, unsigned long DataSize) {
    std::string buffer;
    buffer = Command + "\r\n";
    boost::asio::write(*ftpsocket, boost::asio::buffer(buffer, buffer.length()));
    ReceiveResponce(DesiredResponceCode);
    ReceiveRaw(Responce, DataSize);
}

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

void FTPClient::LeavePassiveMode() {
    receiversocket->close();
    delete receiversocket;
}

unsigned long FTPClient::GetLileList(const std::string &Path, std::list<std::string> &FileList) {
    unsigned long filescount(0);
    try {
        if(!Path.empty()) {
            std::string command = (boost::format("CWD %1%") % Path).str();
            ExecuteCommand(command, 250);
            if(!IsResponceCode(250))
                return 0;
        }
        ExecuteCommand("PASV", 227);
        if(IsResponceCode(227)) {
            std::string host;
            std::string connectdata = GetResponceData(227);
            if (!connectdata.empty()) {
                unsigned short port = ParseForPort(connectdata, host);
                if (EnterPassiveMode(host, port)) {
                    std::string fileleist;
                    ExecuteCommand("NLST", fileleist, 226, 0);
                    if (IsResponceCode(226)) {
                        boost::replace_all(fileleist, "\r", "");
                        boost::split(FileList, fileleist, boost::is_any_of("\n"));
                        filescount = FileList.size();
                    }
                    LeavePassiveMode();
                }
            }
        }
    } catch(Exception &e) {
        filescount = 0;
    }
    return filescount;
}

bool FTPClient::GetFile(const std::string &Filename, std::string &FileContent) {
    bool result(false);
    ExecuteCommand("PASV", 227);
    if(IsResponceCode(227)) {
        std::string host;
        std::string connectdata = GetResponceData(227);
        if (!connectdata.empty()) {
            unsigned short port = ParseForPort(connectdata, host);
            std::string getsizecommand = (boost::format("SIZE %1%") % Filename).str();
            ExecuteCommand(getsizecommand, 213);
            if (IsResponceCode(213)) {
                std::string filesizestr = GetResponceData(213);
                boost::replace_all(filesizestr, " ", "");
            if (EnterPassiveMode(host, port)) {
//                ExecuteCommand("TYPE A", 200);
//                ExecuteCommand("TYPE A", 200);
//                if (IsResponceCode(200)) {
//                    ExecuteCommand("STRU F", 200);
//                    if (IsResponceCode(200)) {
                        unsigned long filesize = boost::lexical_cast<unsigned long>(filesizestr);
                        std::string command = (boost::format("RETR %1%") % Filename).str();
                        ExecuteCommand(command, FileContent, 150, filesize);
                        if(IsResponceCode(150)) {
                            result = true;
//                        }
                    }
                }
            }
        }
    }
    return result;
}

unsigned short FTPClient::ParseForPort(const std::string &Message, std::string &Host) {
    std::regex re("([0-9]*),([0-9]*),([0-9]*),([0-9]*),([0-9]*),([0-9]*)");
    std::smatch match;
    if (std::regex_search(Message, match, re) && (match.size() >= 7)) {
        unsigned short first = boost::lexical_cast<unsigned short>(match[5]);
        unsigned short second = boost::lexical_cast<unsigned short>(match[6]);
        unsigned short hostfirst = boost::lexical_cast<unsigned short>(match[1]);
        unsigned short hostsecond = boost::lexical_cast<unsigned short>(match[2]);
        unsigned short hostthird = boost::lexical_cast<unsigned short>(match[3]);
        unsigned short hostfourth = boost::lexical_cast<unsigned short>(match[4]);
        Host = (boost::format("%1%.%2%.%3%.%4%") % hostfirst % hostsecond % hostthird % hostfourth).str();
        return (first << 8) + second;
    } else {
        return 21;
    }
}
