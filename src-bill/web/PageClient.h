#pragma once

#include "BasePage.h"
#include "../classes/BlackListLocal.h"
#include "../classes/BlackListGlobal.h"
#include "../classes/BlackListTrunk.h"
#include "../threads/ThreadCurrentCalls.h"
#include "../threads/ThreadRemoteLoader.h"
#include "../data/DataContainer.h"
#include "../data/DataBillingContainer.h"
#include "../data/DataCurrentCallsContainer.h"

class PageClient : public BasePage {
public:
    bool canHandle(std::string &path) {
        return path == "/client";
    }
    void render(std::stringstream &html, map<string, string> &parameters) {
        renderHeader(html);

        int client_id = atoi(parameters["id"].c_str());

        PreparedData preparedData;
        auto data = DataContainer::instance();
        if (!data->prepareData(preparedData, time(nullptr))) {
            return;
        };

        auto billingData = DataBillingContainer::instance();
        if (!billingData->ready()) {
            return;
        }

        auto currentCallsData = DataCurrentCallsContainer::instance();


        auto client = preparedData.client->find(client_id);
        if (client == nullptr) {
            html << "Client " << client_id << " not found";
            return;
        }

        double sum_month, sum_day, sum_balance;
        double sum_month2, sum_day2, sum_balance2;
        bool client_disabled_global, client_disabled_local;
        ClientCounterObj &clientCounter = billingData->clientCounter.get()->get(client_id);
        sum_month = clientCounter.sumMonth();
        sum_day = clientCounter.sumDay();
        sum_balance = clientCounter.sumBalance();
        client_disabled_global = clientCounter.disabled_global;
        client_disabled_local = clientCounter.disabled_local;


        ClientCounterObj &c2 = currentCallsData->getClientCounter()->get(client_id);
        sum_balance2 = c2.sumBalance();
        sum_day2 = c2.sumDay();
        sum_month2 = c2.sumMonth();

        double sum_month_global = 0, sum_day_global = 0, sum_balance_global = 0;
        if (data->globalCounters.ready()) {
            auto globalCounter = data->globalCounters.get()->find(client_id);
            if (globalCounter) {
                sum_balance_global += globalCounter->sumBalance();
                sum_day_global += globalCounter->sumDay();
                sum_month_global += globalCounter->sumMonth();
            }
        }


        html << "Client Id: <b>" << client->id << "</b><br/>\n";
        html << "-----<br/>\n";

        if (client_disabled_local)
            html << "Client disabled LOCAL<br/>\n";

        if (client_disabled_global)
            html << "Client disabled GLOBAL<br/>\n";

        html << "-----<br/>\n";


        {
            auto bl = BlackListLocal::instance();
            auto i = bl->blacklist.begin();
            while (i != bl->blacklist.end()) {
                auto serviceNumber = preparedData.serviceNumber->find(atoll(i->first.c_str()), time(nullptr));
                if (serviceNumber != nullptr && serviceNumber->client_account_id == client_id) {
                    html << "Locked Local <b>number</b>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<b>";
                    html << i->first;
                    html << "</b><br/>\n";
                }
                ++i;
            }
        }

        {
            auto bl = BlackListGlobal::instance();
            auto i = bl->blacklist.begin();
            while (i != bl->blacklist.end()) {
                auto serviceNumber = preparedData.serviceNumber->find(atoll(i->first.c_str()), time(nullptr));
                if (serviceNumber != nullptr && serviceNumber->client_account_id == client_id) {
                    html << "Locked Global <b>number</b>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<b>";
                    html << i->first;
                    html << "</b><br/>\n";
                }
                ++i;
            }
        }

        {
            auto bl = BlackListTrunk::instance();
            auto i = bl->blacklist.begin();
            while (i != bl->blacklist.end()) {
                auto trunk = preparedData.trunkByName->find(i->first.c_str());
                if (trunk == nullptr) {
                    trunk = preparedData.trunkByAlias->find(i->first.c_str());
                }
                if (trunk != nullptr) {
                    auto serviceTrunk = preparedData.serviceTrunk->find(trunk->id, time(nullptr));
                    if (serviceTrunk != nullptr && serviceTrunk->client_account_id == client_id) {
                        html << "Locked Trunk <b>trunk</b>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<b>";
                        html << i->first;
                        html << "</b><br/>\n";
                    }
                }
                ++i;
            }
        }


        html << "-----<br/>\n";

        if (client->hasCreditLimit()) {
            html << "Balance avaiable: <b>" << string_fmt("%.2f", client->balance + client->credit + sum_balance + sum_balance2 + sum_balance_global) << "</b> = ";
            html << string_fmt("%.2f", client->balance) << " (balance) + " << string_fmt("%d", client->credit) << " (credit) + " << string_fmt("%.2f", sum_balance) << " (local) + " << string_fmt("%.2f", sum_balance2) << " (current) + " << string_fmt("%.2f", sum_balance_global) << " (global) <br/>\n";
        }

        if (client->hasDailyLimit()) {
            html << "Daily avaiable: <b>" << string_fmt("%.2f", client->limit_d + sum_day + sum_day2 + sum_day_global) << "</b> = ";
            html << string_fmt("%d", client->limit_d) << " (limit_d) + " << string_fmt("%.2f", sum_day) << " (local) + " << string_fmt("%.2f", sum_day2) << " (current) + " << string_fmt("%.2f", sum_day_global) << " (global) <br/>\n";
        }

        if (client->hasMonthlyLimit()) {
            html << "Monthly avaiable: <b>" << string_fmt("%.2f", client->limit_m + sum_month + sum_month2 + sum_month_global) << "</b> = ";
            html << string_fmt("%d", client->limit_m) << " (limit_m) + " << string_fmt("%.2f", sum_month) << " (local) + " << string_fmt("%.2f", sum_month2) << " (current) + " << string_fmt("%.2f", sum_month_global) << " (global) <br/>\n";
        }

        html << "-----<br/>\n";

        if (client->disabled) {
            html << "Voip disabled: <b>" << "true" << "</b><br/>\n";
        }

        html << "-----<br/>\n";

        html << "Last account date: <b>" << string_time(client->amount_date) << "</b><br/>\n";
        html << "Last payed month: <b>" << string_date(client->last_payed_month) << "</b><br/>\n";

        html << "-----<br/>\n";

        html << "Sum from account: <b>" << string_fmt("%.2f", sum_balance + sum_balance2 + sum_balance_global) << "</b> = " << string_fmt("%.2f", sum_balance) << " (local) + " << string_fmt("%.2f", sum_balance2) << " (current) + " << string_fmt("%.2f", sum_balance_global) << " (global) <br/>\n";
        html << "Sum Day: <b>" << string_fmt("%.2f", sum_day + sum_day2 + sum_day_global) << "</b> = " << string_fmt("%.2f", sum_day) << " (local) + " << string_fmt("%.2f", sum_day2) << " (current) + " << string_fmt("%.2f", sum_day_global) << " (global) <br/>\n";
        html << "Sum Month: <b>" << string_fmt("%.2f", sum_month + sum_month2 + sum_month_global) << "</b> = " << string_fmt("%.2f", sum_month) << " (local) + " << string_fmt("%.2f", sum_month2) << " (current) + " << string_fmt("%.2f", sum_month_global) << " (global) <br/>\n";

        html << "-----<br/>\n";



        int calls_count = 0;
        auto calls = DataCurrentCallsContainer::instance()->getCallsWaitingSaving();
        for (auto &call : *calls.get()) {
            if (call.account_id != client_id) continue;
            calls_count++;
        }

        html << "Current calls: <b>" << calls_count << "</b><br/>\n";
        html << "<br/>\n";

        if (calls_count > 0) {
            html << "<table width=100% border=1 cellspacing=0>\n";
            html << "<tr>";
            html << "<th></th>";
            html << "<th>connect_time</th>";
            html << "<th>src_number</th>";
            html << "<th>dst_number</th>";
            html << "<th>billed_time</th>";
            html << "<th>rate</th>";
            html << "<th>cost</th>";
            html << "<th>account</th>";
            html << "<th>number</th>";
            html << "<th>pricelist_id</th>";
            html << "<th>prefix</th>";
            html << "<th>dest</th>";
            html << "<th>geo_id</th>";
            html << "</tr>\n";
            auto calls = DataCurrentCallsContainer::instance()->getCallsWaitingSaving();
            for (auto &call : *calls.get()) {
                if (call.account_id != client_id) continue;
                html << "<tr>";
                html << "<td>" << (call.orig ? "orig" : "term") << "</td>";
                html << "<td>" << string_time(call.connect_time) << "</td>";
                html << "<td>" << call.src_number << "</td>";
                html << "<td>" << call.dst_number << "</td>";
                html << "<td>" << call.billed_time << "</td>";
                html << "<td>" << call.rate << "</td>";
                html << "<td>" << call.cost << "</td>";
                html << "<td>" << call.account_id << "</td>";
                if (call.trunk_service_id != 0) {
                    html << "<td>trunk " << call.trunk_service_id << "</td>";
                } else if (call.number_service_id != 0) {
                    html << "<td>number " << call.number_service_id << "</td>";
                } else {
                    html << "<td></td>";
                }
                html << "<td>" << call.pricelist_id << "</td>";
                html << "<td>" << call.prefix << "</td>";
                html << "<td>" << call.destination_id << (call.mob ? " mob" : "") << "</td>";
                html << "<td>" << call.geo_id << "</td>";
                html << "</tr>\n";
            }
            html << "</table>\n";
        }

    }
};