#pragma once

#include "BaseTest.h"

class ComparisonTest : public BaseTest {

    virtual bool setResult   (unsigned int index) = 0;
    virtual bool checkResult (unsigned int index) = 0;

public:
    virtual unsigned int getResultsNumber () const = 0;

    virtual bool testAllResults ();
};

