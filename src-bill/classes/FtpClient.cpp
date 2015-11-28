#include "FtpClient.h"

#include "Exception.h"

#include <curl/curl.h>
#include <boost/algorithm/string.hpp>

FtpClient::FtpClient() {
}

FtpClient::FtpClient(std::string host, std::string username, std::string password) {
    this->host = host;
    this->username = username;
    this->password = password;
}

void FtpClient::setHost(std::string host) {
    this->host = host;
}

void FtpClient::setUserName(std::string username) {
    this->username = username;
}

void FtpClient::setPassword(std::string password) {
    this->password = password;
}

static size_t
ftp_client_write_data(void *ptr, size_t size, size_t nmemb, void *data)
{
    std::string * str = (std::string *)data;
    str->append((char *)ptr, size * nmemb);
    return size * nmemb;
}

void FtpClient::ls(std::string uri, std::vector<std::string> &files) {
    std::string data;

    fetch(uri, data);

    std::list<std::string> tmpFiles;
    boost::split(tmpFiles, data, boost::is_any_of("\n"));

    for (auto tmpFile : tmpFiles) {
        std::vector<std::string> tmpColumns;
        boost::split(tmpColumns, tmpFile, boost::is_any_of(" "));
        string fileName = tmpColumns.at(tmpColumns.size() - 1);
        if (fileName.size() > 0) {
            files.push_back(tmpColumns.at(tmpColumns.size() - 1));
        }
    }
}

void FtpClient::fetch(std::string uri, std::string &data) {
    CURL *curl;
    CURLcode res;

    curl = curl_easy_init();
    if (!curl) {
        throw Exception("curl_easy_init", "FtpClient::fetch");
    }

    std::string url = "ftp://" + username + ":" + password + "@" + host + uri;

    /* Get a file listing from sunet */
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, ftp_client_write_data);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &data);

    res = curl_easy_perform(curl);
    /* Check for errors */
    if(res != CURLE_OK) {
        string error(curl_easy_strerror(res));
        curl_easy_cleanup(curl);

        throw Exception("curl_easy_perform() failed: " + error, "FtpClient::fetch");
    }

    /* always cleanup */
    curl_easy_cleanup(curl);
}

