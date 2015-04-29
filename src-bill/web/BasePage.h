#pragma once

#include <string>

class BasePage {
public:
    virtual bool canHandle(std::string &path) = 0;
    virtual void render(std::stringstream &html, map<string, string> &parameters) = 0;

    void renderHeader(std::stringstream &html) {
        html << "<style>\n";
        html << "* {font-family:monospace !important;}\n";
        html << "td { text-align: center }\n";
        html << "</style>\n";

        html << " <a href='/'>home</a> ";
        html << " <a href='/config'>config</a> ";
        html << string_time(time(NULL)) << " ";

        AppStatus status = app().getStatus();
        AppStatus real_status = app().getRealStatus();
        html << "<b>" << AppStatusNames[real_status] << "</b>";
        if (status != real_status) {
            html << " (" << AppStatusNames[status] << ")";
        }

        html << " <hr/> ";
    }
};