#pragma once

#include "Exception.h"

class CalcException : public Exception
{
public:
    CalcException() : Exception() { }

    CalcException(const char * _what) : Exception(_what) { }

    CalcException(const string &_what) : Exception(_what) { }
};