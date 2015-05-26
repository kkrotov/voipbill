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

        html << "<table border=1 width=100%>\n";
        html << "<tr>\n";
        html << "<th>Client</th>\n";
        html << "<th>Month</th>\n";
        html << "<th>Month Sum</th>\n";
        html << "<th>Day</th>\n";
        html << "<th>Day Sum</th>\n";
        html << "<th>Date</th>\n";
        html << "<th>Sum</th>\n";
        html << "<th>Lock Local</th>\n";
        html << "<th>Lock Global</th>\n";
        html << "<th>Balance avaiable</th>\n";
        html << "<th>Daily avaiable</th>\n";
        html << "<th>Monthly avaiable</th>\n";
        html << "</tr>\n";
        for (auto it : clientCounter->counter) {
            int key = it.first;
            ClientCounterObj &value = it.second;
            double sum_month = value.sumMonth();
            double sum_day = value.sumDay();
            double sum_balance = value.sumBalance();

            if (sum_month == 0 && sum_day == 0 && sum_balance == 0 && !value.disabled_local && !value.disabled_global) continue;

            auto client = clientList->find(value.client_id);
            if (client == nullptr) continue;

            double sum_month_global = 0, sum_day_global = 0, sum_balance_global = 0;
            if (data->globalCounters.ready()) {
                auto globalCounter = data->globalCounters.get()->find(value.client_id);
                if (globalCounter) {
                    sum_balance_global += globalCounter->sumBalance();
                    sum_day_global += globalCounter->sumDay();
                    sum_month_global += globalCounter->sumMonth();
                }
            }

            html << "<tr>\n";
            html << "<td><a href='/client?id=" << key << "'>" << key << "</a></td>\n";
            html << "<td>" << string_date(value.amount_month) << "</td>\n";
            html << "<td>" << sum_month << "</td>\n";
            html << "<td>" << string_date(value.amount_day) << "</td>\n";
            html << "<td>" << sum_day << "</td>\n";
            html << "<td>" << string_time(client->amount_date) << "</td>\n";
            html << "<td>" << sum_balance << "</td>\n";
            html << "<td>" << (value.disabled_local ? "TRUE" : "false")  << "</td>\n";
            html << "<td>" << (value.disabled_global ? "TRUE" : "false") << "</td>\n";

            if (client != nullptr && client->hasCreditLimit()) {
                html << "<td>" << string_fmt("%.2f", client->balance + client->credit + sum_balance + sum_balance_global) << "</td>\n";
            } else {
                html << "<td>-</td>\n";
            }

            if (client != nullptr && client->hasDailyLimit()) {
                html << "<td>" << string_fmt("%.2f", client->limit_d + sum_day + sum_day_global) << "</td>\n";
            } else {
                html << "<td>-</td>\n";
            }

            if (client != nullptr && client->hasMonthlyLimit()) {
                html << "<td>" << string_fmt("%.2f", client->limit_m + sum_month + sum_month_global) << "</td>\n";
            } else {
                html << "<td>-</td>\n";
            }

            html << "</tr>\n";
        }
        html << "</table>\n";

    }
};