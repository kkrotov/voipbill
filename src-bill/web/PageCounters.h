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
        auto organizationList = data->organization.get();
        auto clientCounter = billingData->clientCounter.get();

        if (clientList == nullptr) return;
        if (organizationList == nullptr) return;
        if (clientCounter == nullptr) return;

        vector<ClientCounterObj> clients;
        clientCounter->getClients(clients);

        html << "<table border=1>\n";
        html << "<tr>\n";
        html << "<th nowrap>Client</th>\n";
        html << "<th nowrap>Month</th>\n";
        html << "<th nowrap>Month Sum</th>\n";
        html << "<th nowrap>Day</th>\n";
        html << "<th nowrap>Day Sum</th>\n";
        html << "<th nowrap>Date</th>\n";
        html << "<th nowrap>Sum</th>\n";
        html << "<th nowrap>Balance avaiable</th>\n";
        html << "<th nowrap>Daily avaiable</th>\n";
        html << "<th nowrap>Monthly avaiable</th>\n";
        html << "<th nowrap>Voip disabled</th>\n";
        html << "</tr>\n";
        for (ClientCounterObj &cc : clients) {
            int client_id = cc.client_id;

            double tax_rate = 0;

            auto client = clientList->find(client_id);
            if (client != nullptr) {
                auto organization = organizationList->find(client->organization_id, time(nullptr));
                if (organization != nullptr) {
                    tax_rate = organization->tax_rate / 100.0;
                }
            }

            double sum_month = cc.sumMonth(tax_rate);
            double sum_day = cc.sumDay(tax_rate);
            double sum_balance = cc.sumBalance(tax_rate);



            if (abs(sum_month) <= 0.0001 && abs(sum_day) <= 0.0001 && abs(sum_balance) <= 0.0001) continue;


            double sum_month_global = 0, sum_day_global = 0, sum_balance_global = 0;
            if (data->globalCounters.ready()) {
                auto globalCounter = data->globalCounters.get()->find(client_id);
                if (globalCounter) {
                    sum_balance_global += globalCounter->sumBalance(tax_rate);
                    sum_day_global += globalCounter->sumDay(tax_rate);
                    sum_month_global += globalCounter->sumMonth(tax_rate);
                }
            }

            html << "<tr>\n";
            html << "<td nowrap><a href='/client?id=" << client_id << "'>" << client_id << "</a></td>\n";
            html << "<td nowrap>" << string_date(cc.amount_month) << "</td>\n";
            html << "<td nowrap>" << sum_month << "</td>\n";
            html << "<td nowrap>" << string_date(cc.amount_day) << "</td>\n";
            html << "<td nowrap>" << sum_day << "</td>\n";
            html << "<td nowrap>" << string_time(cc.amount_date) << "</td>\n";
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