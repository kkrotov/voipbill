#pragma once

#include "BasePage.h"
#include "../data/DataContainer.h"
#include "../data/DataBillingContainer.h"

class PageLocks : public BasePage {
public:
    bool canHandle(std::string &path) {
        return path == "/locks";
    }
    void render(std::stringstream &html, map<string, string> &parameters) {
        renderHeader(html);

        auto data = DataContainer::instance();
        auto billingData = DataBillingContainer::instance();

        auto clientList = data->client.get();
        auto clientCounter = billingData->clientCounter.get();
        auto clientLock = billingData->clientLock.get();

        if (clientList == nullptr) return;
        if (clientCounter == nullptr) return;
        if (clientLock == nullptr || !clientLock->isReady) return;

        html << "<table border=1>\n";
        html << "<tr>\n";
        html << "<th nowrap>Client</th>\n";
        html << "<th nowrap>Lock Local</th>\n";
        html << "<th nowrap>Lock Global</th>\n";
        html << "<th nowrap>Balance avaiable</th>\n";
        html << "<th nowrap>Daily avaiable</th>\n";
        html << "<th nowrap>Monthly avaiable</th>\n";
        html << "<th nowrap>Voip disabled</th>\n";
        html << "</tr>\n";

        vector<ClientLockObj> locks;
        clientLock->getLocks(locks);

        for (auto lock : locks) {
            int client_id = lock.client_id;
            ClientCounterObj value = clientCounter->get(client_id);
            double sum_month = value.sumMonth();
            double sum_day = value.sumDay();
            double sum_balance = value.sumBalance();

            auto client = clientList->find(client_id);

            double sum_month_global = 0, sum_day_global = 0, sum_balance_global = 0;
            if (data->globalCounters.ready()) {
                auto globalCounter = data->globalCounters.get()->find(client_id);
                if (globalCounter) {
                    sum_balance_global += globalCounter->sumBalance();
                    sum_day_global += globalCounter->sumDay();
                    sum_month_global += globalCounter->sumMonth();
                }
            }

            html << "<tr>\n";
            html << "<td nowrap><a href='/client?id=" << client_id << "'>" << client_id << "</a></td>\n";
            html << "<td nowrap>" << (lock.disabled_local ? "LOCK" : "-")  << "</td>\n";
            html << "<td nowrap>" << (lock.disabled_global ? "LOCK" : "-") << "</td>\n";

            if (client != nullptr && client->hasCreditLimit()) {
                html << "<td nowrap>" << string_fmt("%.2f", client->balance + client->credit + sum_balance + sum_balance_global) << "</td>\n";
            } else {
                html << "<td nowrap>-</td>\n";
            }

            if (client != nullptr && client->hasDailyLimit()) {
                html << "<td nowrap>" << string_fmt("%.2f", client->limit_d + sum_day + sum_day_global) << "</td>\n";
            } else {
                html << "<td nowrap>-</td>\n";
            }

            if (client != nullptr && client->hasMonthlyLimit()) {
                html << "<td nowrap>" << string_fmt("%.2f", client->limit_m + sum_month + sum_month_global) << "</td>\n";
            } else {
                html << "<td nowrap>-</td>\n";
            }

            if (client != nullptr && client->disabled) {
                html << "<td nowrap>" << "DISABLED" << "</td>\n";
            } else {
                html << "<td nowrap>-</td>\n";
            }


            html << "</tr>\n";
        }
        html << "</table>\n";

    }
};