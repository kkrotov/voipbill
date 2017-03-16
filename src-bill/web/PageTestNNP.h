#pragma once

#include "BasePage.h"
#include "../models/Cdr.h"
#include "../models/Call.h"
#include "../classes/Repository.h"
#include "../classes/DbException.h"
#include "../classes/Billing.h"
#include "../classes/BillingCall.h"
#include "../classes/nnp/NNPResolver.h"
#include <string>
#include <regex>

using namespace std;

class PageTestNNP : public BasePage {
    Repository repository;
    bool is_need_debug_trace = false;
    bool use_simple_format = false;

public:
    bool canHandle(std::string &path) {
        return path == "/test/nnpcalc";
    }

    void render(std::stringstream &html, map<string, string> &parameters) ;
    bool getMainPrefixList (int prefixlist_id, Prefixlist &prefixlist);
    bool savePrefixList (int prefixlist_id, std::vector<PhoneNumber> phoneList);

private:
    void getDestinationByNum (std::stringstream &html, map<string, string> &parameters);
    void getNumberRangeByNum (std::stringstream &html, map<string, string> &parameters);
    void getBestGeoRoute (std::stringstream &html, map<string, string> &parameters);
    void getBestPriceRoute (std::stringstream &html, map<string, string> &parameters);
    void getPrefixListByDestinationID (std::stringstream &html, map<string, string> &parameters);
    void fillNNPPrefixList (std::stringstream &html, map<string, string> &parameters);
};