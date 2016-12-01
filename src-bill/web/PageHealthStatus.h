#pragma once

#include "BasePage.h"

static const char *statusString[] = {"UNKNOWN", "OK", "WARNING", "ERROR", "CRITICAL"};

class PageHealthStatus : public BasePage {

public:
    bool canHandle(std::string &path) ;
    void healthStatusHeader (stringstream &html);
    void healthStatusData (stringstream &html, SystemStatus statusData);
    void render(std::stringstream &html, map<string, string> &parameters);
};