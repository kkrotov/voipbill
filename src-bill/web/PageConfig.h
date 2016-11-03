#pragma once

#include "BasePage.h"
#include "../classes/Repository.h"

class PageConfig : public BasePage {
public:
    bool canHandle(std::string &path) {
        return path == "/config";
    }

    void render(std::stringstream &html, map<string, string> &parameters);
};