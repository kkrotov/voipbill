#pragma once

#include "BasePage.h"
#include "../classes/Repository.h"

class PageHealthCheck : public BasePage {

    Repository repository;
    vector<pair<int,string>> getRegionList();
    Json::Value getJsonRegionList();

public:
    bool canHandle(std::string &path);
    void render(std::stringstream &html, map<string, string> &parameters);
};
