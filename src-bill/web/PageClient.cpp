#include "PageClient.h"

void PageClient::render_client_locks(std::stringstream &html, Client *client) {
    if (client == nullptr) {
        html << "Passed a null pointer - Client *client ";
        return;
    }
    int client_id = client->id;

    ClientLockObj clientLock = repository.billingData->clientLock.get()->get(client_id);

    if (clientLock.disabled_local)
        html << "Blocked MGMN<br/>\n";

    if (clientLock.disabled_global)
        html << "Blocked GLOBAL<br/>\n";

    if (clientLock.is_finance_block)
        html << "Blocked FINANCE<br/>\n";

    if (clientLock.is_overran)
        html << "Blocked OVERRAN<br/>\n";

    html << "-----<br/>\n";

    {
        auto bl = BlackListLocal::instance();
        lock_guard<Spinlock> guard(bl->lock);
        for (auto phone : bl->blacklist) {
            auto serviceNumber = repository.getServiceNumber(atoll(phone.c_str()));
            if (serviceNumber != nullptr && serviceNumber->client_account_id == client_id) {
                html << "Locked Local <b>number</b>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<b>";
                html << phone;
                html << "</b><br/>\n";
            }
        }
    }

    {
        auto bl = BlackListGlobal::instance();
        lock_guard<Spinlock> guard(bl->lock);
        for (auto phone : bl->blacklist) {
            auto serviceNumber = repository.getServiceNumber(atoll(phone.c_str()));
            if (serviceNumber != nullptr && serviceNumber->client_account_id == client_id) {
                html << "Locked Global <b>number</b>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<b>";
                html << phone;
                html << "</b><br/>\n";
            }
        }
    }

    {
        auto bl = BlackListTrunk::instance();
        lock_guard<Spinlock> guard(bl->lock);
        for (auto phone : bl->blacklist) {
            auto trunk = repository.getTrunkByName(phone.c_str());
            if (trunk != nullptr) {
                auto serviceTrunk = repository.getServiceTrunk(trunk->id);
                if (serviceTrunk != nullptr && serviceTrunk->client_account_id == client_id) {
                    html << "Locked Trunk <b>trunk</b>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<b>";
                    html << phone;
                    html << "</b><br/>\n";
                }
            }
        }
    }

    html << "-----<br/>\n";


    if (client->is_blocked) {
        html << "Manual Block Global Flag: <b>" << "true" << "</b><br/>\n";
    }

    if (client->disabled) {
        html << "Manual Block MGMN Flag: <b>" << "true" << "</b><br/>\n";
    }

    html << "-----<br/>\n";
}

void PageClient::render_client_current_calls(std::stringstream &html, Client *client) {
    if (client == nullptr) {
        html << "Passed a null pointer - Client *client ";
        return;
    }
    int client_id = client->id;

    int calls_count = 0;
    auto calls = repository.currentCalls->getCallsWaitingSaving();
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
        auto calls = repository.currentCalls->getCallsWaitingSaving();
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

void PageClient::render_client_balance_indicators(std::stringstream &html, Client *client) {
    if (client == nullptr) {
        html << "Passed a null pointer - Client *client ";
        return;
    }
    int client_id = client->id;

    double vat_rate = repository.getVatRate(client);

    double sum_month, sum_day, sum_balance;
    double sum_month2, sum_day2, sum_balance2;

    sum_month = repository.billingData->statsAccountGetSumMonth(client_id, vat_rate);
    sum_day = repository.billingData->statsAccountGetSumDay(client_id, vat_rate);
    sum_balance = repository.billingData->statsAccountGetSumBalance(client_id, vat_rate);


    auto statsAccount2 = repository.currentCalls->getStatsAccount().get();
    sum_balance2 = statsAccount2->getSumBalance(client_id, vat_rate);
    sum_day2 = statsAccount2->getSumDay(client_id, vat_rate);
    sum_month2 = statsAccount2->getSumMonth(client_id, vat_rate);

    double sum_month_global = 0, sum_day_global = 0, sum_balance_global = 0;
    if (repository.data->globalCounters.ready()) {
        auto globalCounter = repository.data->globalCounters.get()->find(client_id);
        if (globalCounter) {
            sum_balance_global += globalCounter->sumBalance(vat_rate);
            sum_day_global += globalCounter->sumDay(vat_rate);
            sum_month_global += globalCounter->sumMonth(vat_rate);
        }
    }

    if (client->hasCreditLimit()) {
        html << "Balance available: <b>" <<
        string_fmt("%.2f", client->balance + client->credit + sum_balance + sum_balance2 + sum_balance_global) <<
        "</b> = ";
        html << string_fmt("%.2f", client->balance) << " (balance) + " << string_fmt("%d", client->credit) <<
        " (credit) + " << string_fmt("%.2f", sum_balance) << " (local) + " << string_fmt("%.2f", sum_balance2) <<
        " (current) + " << string_fmt("%.2f", sum_balance_global) << " (global) <br/>\n";
    }

    if (client->hasDailyLimit()) {
        html << "Daily available: <b>" << string_fmt("%.2f", client->limit_d + sum_day + sum_day2 + sum_day_global) <<
        "</b> = ";
        html << string_fmt("%d", client->limit_d) << " (limit_d) + " << string_fmt("%.2f", sum_day) << " (local) + " <<
        string_fmt("%.2f", sum_day2) << " (current) + " << string_fmt("%.2f", sum_day_global) << " (global) <br/>\n";
    }

    html << "-----<br/>\n";

    html << "Last account date: <b>" << (client->amount_date > 0 ? string_time(client->amount_date) : "<none>") <<
    "</b><br/>\n";

    html << "-----<br/>\n";

    html << "Sum from account: <b>" << string_fmt("%.2f", sum_balance + sum_balance2 + sum_balance_global) <<
    "</b> = " << string_fmt("%.2f", sum_balance) << " (local) + " << string_fmt("%.2f", sum_balance2) <<
    " (current) + " << string_fmt("%.2f", sum_balance_global) << " (global) <br/>\n";
    html << "Sum Day: <b>" << string_fmt("%.2f", sum_day + sum_day2 + sum_day_global) << "</b> = " <<
    string_fmt("%.2f", sum_day) << " (local) + " << string_fmt("%.2f", sum_day2) << " (current) + " <<
    string_fmt("%.2f", sum_day_global) << " (global) <br/>\n";
    html << "Sum Month: <b>" << string_fmt("%.2f", sum_month + sum_month2 + sum_month_global) << "</b> = " <<
    string_fmt("%.2f", sum_month) << " (local) + " << string_fmt("%.2f", sum_month2) << " (current) + " <<
    string_fmt("%.2f", sum_month_global) << " (global) <br/>\n";

    html << "-----<br/>\n";

}

void PageClient::render_client_packeges_info(std::stringstream &html, Client *client) {
    if (client == nullptr) {
        html << "Passed a null pointer - Client *client ";
        return;
    }

    if (!repository.data->nnpAccountTariffLight.ready() ||
        !repository.data->serviceNumber.ready() ||
        !repository.data->nnpPackageMinute.ready() ||
        !repository.data->nnpDestination.ready())
        return;

    int client_id = client->id;

    html << "---- active packages with minutes ----<br/>\n";
    vector<NNPAccountTariffLight> nnpAccountTariffLight;
    vector<ServiceNumber> serviceNumber;

    repository.getServiceNumberByClientID(serviceNumber, client_id);
    repository.getActiveNNPAccountTariffLight(nnpAccountTariffLight, client_id);

    for (auto it = serviceNumber.begin(); it != serviceNumber.end(); it++) {
        html << "did:<b>" << it->did << "</b>, lines_count:" << it->lines_count << "<br/>";
        for (auto it2 = nnpAccountTariffLight.begin(); it2 != nnpAccountTariffLight.end(); it2++) {
            if (it2->service_number_id == it->id) {
                vector<NNPPackageMinute> nnpPackageMinute;
                repository.getNNPPackageMinuteByTariff(nnpPackageMinute, it2->nnp_tariff_id);

                if (nnpPackageMinute.size() > 0) {
                    html << "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;";
                    html << "nnp_tariff_id=" << it2->nnp_tariff_id << ",";
                    html << "package_minute_id=" << it2->id << ",";
                    html << "coefficient=" << string_fmt("%.4f", it2->coefficient) << ",";
                    html << "[" << string_time(it2->activate_from) << "," << string_time(it2->deactivate_from) <<
                    "]<br/>";

                    for (auto it3 = nnpPackageMinute.begin(); it3 != nnpPackageMinute.end(); it3++) {

                        NNPDestination *nnpDestination = repository.getNNPDestination(it3->nnp_destination_id);
                        int used_seconds = repository.billingData->statsNNPPackaeMinuteGetUsedSeconds(
                                it2->nnp_tariff_id, it3->id);

                        html << "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;";
                        html << "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;";
                        html << "nnp_destination_id=" << it3->nnp_destination_id << " (<b>";
                        html << nnpDestination->name << "</b>),";
                        html << "minutes in package=" << it3->minute << " ";
                        html << "(effective minutes=" << string_fmt("%.2f", it3->minute * it2->coefficient) << "), ";
                        html << "used minutes=" << string_fmt("%.2f", (double) used_seconds / 60.0) << ",";
                        html << "left minutes=<b>" <<
                        string_fmt("%.2f", it3->minute * it2->coefficient - (double) used_seconds / 60.0) <<
                        "</b>";
                        html << "<br/>";
                    }

                }


            }

        }
    }

    html << "-----<br/>\n";

    return;
}

void PageClient::render(std::stringstream &html, map<string, string> &parameters) {
    renderHeader(html);

    int client_id = atoi(parameters["id"].c_str());

    if (!repository.prepare(time(nullptr))) {
        return;
    };

    if (!repository.billingData->ready()) {
        return;
    }

    auto client = repository.getAccount(client_id);
    if (client == nullptr) {
        html << "Client " << client_id << " not found";
        return;
    }
    html << "Client Id: <b>" << client->id << "</b><br/>\n";
    html << "Account version: <b>" << client->account_version << "</b><br/>\n";
    html << "-----<br/>\n";

    render_client_balance_indicators(html, client);

    if (client->account_version == CALL_ACCOUNT_VERSION_5)
        render_client_packeges_info(html, client);

    render_client_locks(html, client);
    render_client_current_calls(html, client);

}