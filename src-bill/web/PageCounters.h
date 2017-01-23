#pragma once

#include "BasePage.h"
#include "../classes/Repository.h"

class PageCounters : public BasePage {
public:
    bool canHandle(std::string &path) {
        return path == "/counters";
    }
    void render(std::stringstream &html, map<string, string> &parameters) {
        renderHeader("counters",html);

        Repository repository;

        auto clientList = repository.data->client.get();
        auto organizationList = repository.data->organization.get();

        if (clientList == nullptr) return;
        if (organizationList == nullptr) return;

        vector<StatsAccount> clients;
        repository.billingData->statsAccountGetClients(clients);

        html << "<table border=1>\n";
        html << "<tr>\n";
        html << "<th nowrap>Client</th>\n";
        html << "<th nowrap>Month</th>\n";
        html << "<th nowrap>Month Sum</th>\n";
        html << "<th nowrap>Day</th>\n";
        html << "<th nowrap>Day Sum</th>\n";
        html << "<th nowrap>Day MN Sum</th>\n";
        html << "<th nowrap>Date</th>\n";
        html << "<th nowrap>Sum</th>\n";
        html << "<th nowrap>Balance available</th>\n";
        html << "<th nowrap>Daily available</th>\n";
        html << "<th nowrap>Daily MN available</th>\n";
        html << "<th nowrap>Block MGMN Flag</th>\n";
        html << "<th nowrap>Block Global Flag</th>\n";
        html << "</tr>\n";
        for (StatsAccount &cc : clients) {
            int client_id = cc.account_id;

            double vat_rate = 0;

            auto client = clientList->find(client_id);
            if (client != nullptr) {
                auto organization = organizationList->find(client->organization_id, time(nullptr));
                if (organization != nullptr) {
                    vat_rate = organization->vat_rate;
                }
            }

            double sum_month_raw = cc.sumMonth(vat_rate, false);
            double sum_day_raw = cc.sumDay(vat_rate, false);
            double sum_mn_day_raw = cc.sumMNDay(vat_rate, false);
            double sum_month = cc.sumMonth(vat_rate);
            double sum_day = cc.sumDay(vat_rate);
            double sum_mn_day = cc.sumMNDay(vat_rate);
            double sum_balance = cc.sumBalance(vat_rate);

            if (abs(sum_month) <= 0.0001 && abs(sum_day) <= 0.0001 && abs(sum_mn_day) <= 0.0001 && abs(sum_balance) <= 0.0001) continue;


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
            html << "<td nowrap>" << string_date(cc.amount_month, 12) << "</td>\n";
            html << "<td nowrap>" << sum_month_raw << "</td>\n";
            html << "<td nowrap>" << string_date(cc.amount_day, 13) << "</td>\n";
            html << "<td nowrap>" << sum_day_raw << "</td>\n";
            html << "<td nowrap" <<  ( fabs(sum_day_raw) < fabs(sum_mn_day_raw) ? "  style=\"color:Red\" " : " ") << ">" << sum_mn_day_raw << "</td>\n";
            html << "<td nowrap>" << (cc.amount_date>0? string_time(cc.amount_date):"<none>") << "</td>\n";
            html << "<td nowrap>" << sum_balance << "</td>\n";

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
                html << "<td nowrap>" << string_fmt("%.2f", client->limit_d_mn + sum_mn_day + sum_mn_day_global) << "</td>\n";
            } else {
                html << "<td nowrap>-</td>\n";
            }

            if (client != nullptr && client->disabled) {
                html << "<td nowrap>" << "BLOCK MGMN" << "</td>\n";
            } else {
                html << "<td nowrap>-</td>\n";
            }

            if (client != nullptr && client->is_blocked) {
                html << "<td nowrap>" << "BLOCK GLOBAL" << "</td>\n";
            } else {
                html << "<td nowrap>-</td>\n";
            }


            html << "</tr>\n";
        }
        html << "</table>\n";

    }
};