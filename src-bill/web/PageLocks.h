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
        auto organizationList = data->organization.get();
        auto clientCounter = billingData->clientCounter.get();
        auto clientLock = billingData->clientLock.get();

        if (clientList == nullptr) return;
        if (organizationList == nullptr) return;
        if (clientCounter == nullptr) return;
        if (clientLock == nullptr || !clientLock->isReady) return;

        html << "<table border=1>\n";
        html << "<tr>\n";
        html << "<th nowrap>Client</th>\n";
        html << "<th nowrap>Block MGMN</th>\n";
        html << "<th nowrap>Block Global</th>\n";
        html << "<th nowrap>Balance avaiable</th>\n";
        html << "<th nowrap>Daily avaiable</th>\n";
        html << "<th nowrap>Block MGMN Flag</th>\n";
        html << "<th nowrap>Block Global Flad</th>\n";
        html << "</tr>\n";

        vector<ClientLockObj> locks;
        clientLock->getLocks(locks);

        for (auto lock : locks) {
            int client_id = lock.client_id;
            ClientCounterObj value = clientCounter->get(client_id);

            double vat_rate = 0;

            auto client = clientList->find(client_id);
            if (client != nullptr) {
                auto organization = organizationList->find(client->organization_id, time(nullptr));
                if (organization != nullptr) {
                    vat_rate = organization->vat_rate;
                }
            }

            double sum_month = value.sumMonth(vat_rate);
            double sum_day = value.sumDay(vat_rate);
            double sum_balance = value.sumBalance(vat_rate);


            double sum_month_global = 0, sum_day_global = 0, sum_balance_global = 0;
            if (data->globalCounters.ready()) {
                auto globalCounter = data->globalCounters.get()->find(client_id);
                if (globalCounter) {
                    sum_balance_global += globalCounter->sumBalance(vat_rate);
                    sum_day_global += globalCounter->sumDay(vat_rate);
                    sum_month_global += globalCounter->sumMonth(vat_rate);
                }
            }

            html << "<tr>\n";
            html << "<td nowrap><a href='/client?id=" << client_id << "'>" << client_id << "</a></td>\n";
            html << "<td nowrap>" << (lock.disabled_local ? "BLOCK MGNM" : "-")  << "</td>\n";
            html << "<td nowrap>" << (lock.disabled_global ? "BLOCK GLOBAL" : "-") << "</td>\n";

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

            if (client != nullptr && client->disabled) {
                html << "<td nowrap>" << "TRUE" << "</td>\n";
            } else {
                html << "<td nowrap>-</td>\n";
            }

            if (client != nullptr && client->is_blocked) {
                html << "<td nowrap>" << "TRUE" << "</td>\n";
            } else {
                html << "<td nowrap>-</td>\n";
            }

            html << "</tr>\n";
        }
        html << "</table>\n";

    }
};