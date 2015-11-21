#pragma once

#include "BasePage.h"
#include "../classes/Thread.h"

class PageHome : public BasePage {
public:
    bool canHandle(std::string &path) {
        return path == "/";
    }
    void render(std::stringstream &html, map<string, string> &parameters) {
        renderHeader(html);

        html << "<table width=100%>\n";
        bool firstRow = true;
        app().threads.forAllThreads([&](Thread* thread) {
            if (firstRow) {
                thread->threadTotalsHeader(html);
                firstRow = false;
            }
            thread->threadTotalsData(html);
            return true;
        });
        html << "</table>\n";
    }
};