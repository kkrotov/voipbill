#pragma once

#include <string>
#include "common.h"

using namespace std;

class BaseTest {

    BaseTest (BaseTest&);
    BaseTest & operator= (BaseTest&);

protected:

    string lastError;
    CURL *curl;

public:

    BaseTest ();
    ~BaseTest();

    const string & getLastError () const;

    virtual const char * getTestName  () const = 0;

    void resetError ();

    virtual void curlWriteCallback (void *contents_temp, size_t size) = 0;


};
