#pragma once

#include "BasePage.h"

class PageTestRoute : public BasePage {
public:
    bool canHandle(std::string &path) {
        return path == "/test/route";
    }
    void render(std::stringstream &html, map<string, string> &parameters) {
        renderHeader(html);


    }
};