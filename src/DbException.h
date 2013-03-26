#pragma once

#include <string>
#include <cstring>

class DbException : public std::exception
{
    char message[1024];
public:
    explicit DbException(const char * _what){
        strncpy(message, _what, sizeof(message));
    }
    explicit DbException(const std::string &_what){
        strncpy(message, _what.c_str(), sizeof(message));
    }
    virtual const char* what() const throw()
    {
        return (char*)&message;
    }
};
