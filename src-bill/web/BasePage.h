#pragma once

#include <string>
#include <map>
#include <sstream>
#include "../classes/AppBill.h"
#include "../web/http/server.hpp"
#include "../web/http/reply.hpp"


using namespace std;

class BasePage {
public:
    virtual bool canHandle(std::string &path) = 0;
    virtual void render(std::stringstream &html, map<string, string> &parameters) = 0;

    void renderHeader(std::stringstream &html) {
        html << "<style>\n";
        html << "* {font-family:monospace !important;}\n";
        html << "td { text-align: center }\n";
        html << "</style>\n";

        html << " <a href='/'>Home</a> | ";
        html << " <a href='/calls'>Calls</a> | ";
        html << " <a href='/data'>Data</a> | ";
        html << " <a href='/data-billing'>Data billing</a> | ";
        html << " <a href='/counters'>Counters</a> | ";
        html << " <a href='/fmins'>Free minutes</a> | ";
        html << " <a href='/locks'>Locks</a> | ";
        html << " <a href='/blacklist'>Blacklist</a> | ";
        html << " <a href='/sync'>Sync</a> | ";
        html << " <a href='/log'>Log</a> | ";
        html << " <a href='/config'>Config</a> | ";
        html << string_time(time(NULL)) << " ";

        AppStatus status = app().getStatus();
        AppStatus real_status = app().getRealStatus();
        html << "<b>" << AppStatusNames[real_status] << "</b>";
        if (status != real_status) {
            html << " (" << AppStatusNames[status] << ")";
        }

        html << "\n<hr/><br/>\n";
    }
};