#pragma once

#include "BasePage.h"
#include "../classes/Repository.h"

class PageLocks : public BasePage {
public:
    bool canHandle(std::string &path) {
        return path == "/locks";
    }
    void render(std::stringstream &html, map<string, string> &parameters) {
        renderHeader("locks", html);

        Repository repository;

        auto clientList = repository.data->client.get();
        auto organizationList = repository.data->organization.get();
        auto clientLock = repository.billingData->clientLock.get();

        if (clientList == nullptr) return;
        if (organizationList == nullptr) return;
        if (clientLock == nullptr || !clientLock->isReady) return;

        html << "<table border=1>\n";
        html << "<tr>\n";
        html << "<th nowrap>Client</th>\n";
        html << "<th nowrap>Block MGMN</th>\n";
        html << "<th nowrap>Block Global</th>\n";

        html << "<th nowrap>Block is_finance_block</th>\n";
        html << "<th nowrap>Block is_overran</th>\n";
        html << "<th nowrap>Block is_mn_overran</th>\n";

        html << "<th nowrap>Balance available</th>\n";
        html << "<th nowrap>Daily available</th>\n";
        html << "<th nowrap>Daily MN available</th>\n";
        html << "<th nowrap>Block MGMN Flag</th>\n";
        html << "<th nowrap>Block Global Flag</th>\n";
        html << "</tr>\n";

        vector<ClientLockObj> locks;
        clientLock->getLocks(locks);

        for (auto lock : locks) {
            int client_id = lock.client_id;

            double vat_rate = 0;

            auto client = clientList->find(client_id);
            if (client != nullptr) {
                auto organization = organizationList->find(client->organization_id, time(nullptr));
                if (organization != nullptr) {
                    vat_rate = organization->vat_rate;
                }
            }

            double sum_month = repository.billingData->statsAccountGetSumMonth(client_id, vat_rate);
            double sum_day = repository.billingData->statsAccountGetSumDay(client_id, vat_rate);
            double sum_mn_day = repository.billingData->statsAccountGetSumMNDay(client_id, vat_rate);
            double sum_balance = repository.billingData->statsAccountGetSumBalance(client_id, vat_rate);


            double sum_month_global = 0, sum_day_global = 0, sum_balance_global = 0, sum_mn_day_global = 0;
            if (repository.data->globalCounters.ready()) {
                auto globalCounter = repository.data->globalCounters.get()->find(client_id);
                if (globalCounter) {
                    sum_balance_global += globalCounter->sumBalance(vat_rate);
                    sum_day_global += globalCounter->sumDay(vat_rate);
                    sum_mn_day_global += globalCounter->sumMNDay(vat_rate);
                }
            }

            html << "<tr>\n";
            html << "<td nowrap><a href='/client?id=" << client_id << "'>" << client_id << "</a></td>\n";
            html << "<td nowrap>" << (lock.disabled_local ? "BLOCK MGMN" : "-")  << "</td>\n";

            html << "<td nowrap>" << (lock.disabled_global ? "BLOCK GLOBAL" : "-") << "</td>\n";

            html << "<td nowrap>" << (lock.is_finance_block ? "BLOCK FINANCE" : "-")  << "</td>\n";
            html << "<td nowrap>" << (lock.is_overran ? "BLOCK OVERRAN" : "-") << "</td>\n";
            html << "<td nowrap>" << (lock.is_mn_overran ? "BLOCK MN OVERRAN" : "-") << "</td>\n";
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

            if (client != nullptr && client->hasDailyMNLimit()) {
                html << "<td nowrap>" << string_fmt("%.2f", client->limit_d_mn + sum_mn_day + sum_mn_day_global) <<
                "</td>\n";
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