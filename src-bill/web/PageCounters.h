#pragma once

#include "BasePage.h"
#include "../data/DataContainer.h"
#include "../data/DataBillingContainer.h"

class PageCounters : public BasePage {
public:
    bool canHandle(std::string &path) {
        return path == "/counters";
    }
    void render(std::stringstream &html, map<string, string> &parameters) {
        renderHeader(html);

        auto data = DataContainer::instance();
        auto billingData = DataBillingContainer::instance();

        auto clientList = data->client.get();
        auto clientCounter = billingData->clientCounter.get();

        if (clientList == nullptr) return;
        if (clientCounter == nullptr) return;

        html << "<table><tr><th>client</th><th>month</th><th>month sum</th><th>day</th><th>day sum</th><th>date</th><th>sum</th><th>lock local</th><th>lock global</th></tr>";
        for (auto it : clientCounter->counter) {
            int key = it.first;
            ClientCounterObj &value = it.second;
            double sum_month = value.sumMonth();
            double sum_day = value.sumDay();
            double sum_balance = value.sumBalance();


            time_t amount_date = 0;
            auto client = clientList->find(value.client_id);
            if (client != nullptr) {
                amount_date = client->amount_date;
            }

            if (sum_month == 0 && sum_day == 0 && sum_balance == 0 && !value.disabled_local && !value.disabled_global) continue;
            html << "<tr><td>"
                    << "<a href='/client?id=" << key << "'>" << key << "</a></td><td>"
                    << string_date(value.amount_month) << "</td><td>"
                    << sum_month << "</td><td>"
                    << string_date(value.amount_day) << "</td><td>"
                    << sum_day << "</td><td>"
                    << string_time(amount_date) << "</td><td>"
                    << sum_balance << "</td></td>"
                    << (value.disabled_local ? "TRUE" : "false")  << "</td></td>"
                    << (value.disabled_global ? "TRUE" : "false") << "</td></tr>";
        }
        html << "</table>";

    }
};