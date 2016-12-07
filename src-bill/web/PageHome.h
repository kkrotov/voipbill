#pragma once

#include "BasePage.h"
#include "../classes/Thread.h"

class PageHome : public BasePage {
public:
    bool canHandle(std::string &path) {
        return path == "/";
    }
    void render(std::stringstream &html, map<string, string> &parameters) {
        renderHeader("",html);

        bool reset = parameters["action"]=="do_reset";
        html << "<table width=100%>\n";
        bool firstRow = true;
        app().threads.forAllThreads([&](Thread* thread) {
            if (firstRow) {
                thread->threadTotalsHeader(html);
                firstRow = false;
            }
            if (reset)
                thread->resetErrors();

            thread->threadTotalsData(html);
            return true;
        });
        html << "</table>\n";

        html << "<form action=\"\" method=\"POST\">\n"
                "    <button name=\"action\" value=\"do_reset\">Reset errors</button>\n"
                "</form>";
    }
};