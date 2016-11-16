#pragma once

#include "BaseTest.h"
#include <curl/curl.h>
#include "../../src-bill/classes/BDb.h"
#include <vector>
#include <regex>
#include <utility>
#include <string.h>

using namespace std;

/**
 * Класс тестирования функции getNNPDestinationsByRange.
 * Тестирование проводится через url запросы к запущенному биллеру.
 *
 */
class NNPNumberRangeFindTest : public BaseTest {

    NNPNumberRangeFindTest (NNPNumberRangeFindTest&);
    NNPNumberRangeFindTest& operator = (NNPNumberRangeFindTest&);

    const static unsigned int maxIndex = 10 * 1000;

    BDb db;
    CURL *curl;

    string resultRange;
    vector<string> resultPrefix;
    vector<string> resultDestination;

    bool checkRange       (string &number);
    bool checkPrefix      ();
    bool checkDestination ();
    void getDestinations  (vector<string> & dests);

public:

    NNPNumberRangeFindTest ();

    virtual bool setResult   (unsigned int index) override;
    virtual bool checkResult (unsigned int index) override;

    virtual unsigned int getMaxIndex () const override;
    virtual const char*  getTestName () const override;

    void setCurlResult (const char* res, vector<string>* prefix, vector<string>* destination);

    ~NNPNumberRangeFindTest();
};
