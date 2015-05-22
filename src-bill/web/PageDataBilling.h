#pragma once

#include "BasePage.h"
#include "../data/DataBillingContainer.h"

using namespace std;

class PageDataBilling : public BasePage {
public:
    bool canHandle(std::string &path) {
        return path == "/data-billing";
    }
    void render(std::stringstream &html, map<string, string> &parameters) {
        renderHeader(html);

        auto billingData = DataBillingContainer::instance();

        billingData->html(html);
    }
};