#pragma once

#include "BasePage.h"
#include "../classes/BlackListLocal.h"
#include "../classes/BlackListGlobal.h"
#include "../classes/BlackListTrunk.h"
#include "../classes/BlackListAntiFraudDisable.h"
#include "../classes/Repository.h"

class PageBlacklist : public BasePage {
public:
    bool canHandle(std::string &path) {
        return path == "/blacklist";
    }
    void render(std::stringstream &html, map<string, string> &parameters) {
        renderHeader(html);

        Repository repository;
        if (!repository.prepare(time(nullptr))) {
            return;
        }

        auto blacklist_global = BlackListGlobal::instance();
        auto blacklist_local = BlackListLocal::instance();
        auto blacklist_trunk = BlackListTrunk::instance();
        auto blacklist_anti_fraud_disable = BlackListAntiFraudDisable::instance();

        html << "<table>\n";
        html << "<tr>\n";

        ServiceNumber * usage;
        {
            lock_guard<Spinlock> guard(blacklist_local->lock);

            if (blacklist_local->list_to_add.size() > 0) {
                html << "<td nowrap valign=top>\n";
                html << "BlackListLocal to Add: <b>" << blacklist_local->list_to_add.size() << "</b><br/>\n";
                for (auto phone : blacklist_local->list_to_add) {
                    html << "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;";
                    html << "<b>" << phone << "</b>";
                    if ((usage = repository.getServiceNumber(phone.c_str())) != 0)
                        html << " / " << "<a href='/client?id=" << usage->client_account_id << "'>" << usage->client_account_id << "</a>";
                    html << "<br/>\n";
                }
                html << "</td>\n";
            }

            if (blacklist_local->list_to_del.size() > 0) {
                html << "<td nowrap valign=top>\n";
                html << "BlackListLocal to Del: <b>" << blacklist_local->list_to_del.size() << "</b><br/>\n";
                for (auto phone : blacklist_local->list_to_del) {
                    html << "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;";
                    html << "<b>" << phone << "</b>";
                    if ((usage = repository.getServiceNumber(phone.c_str())) != 0)
                        html << " / " << "<a href='/client?id=" << usage->client_account_id << "'>" << usage->client_account_id << "</a>";
                    html << "<br/>\n";
                }
                html << "</td>\n";
            }
        }

        {
            lock_guard<Spinlock> guard(blacklist_global->lock);

            if (blacklist_global->list_to_add.size() > 0) {
                html << "<td nowrap valign=top>\n";
                html << "BlackListGlobal to Add: <b>" << blacklist_global->list_to_add.size() << "</b><br/>\n";
                for (auto phone : blacklist_global->list_to_add) {
                    html << "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;";
                    html << "<b>" << phone << "</b>";
                    if ((usage = repository.getServiceNumber(phone.c_str())) != 0)
                        html << " / " << "<a href='/client?id=" << usage->client_account_id << "'>" << usage->client_account_id << "</a>";
                    html << "<br/>\n";
                }
                html << "</td>\n";
            }

            if (blacklist_global->list_to_del.size() > 0) {
                html << "<td nowrap valign=top>\n";
                html << "BlackListGlobal to Del: <b>" << blacklist_global->list_to_del.size() << "</b><br/>\n";
                for (auto phone : blacklist_global->list_to_del) {
                    html << "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;";
                    html << "<b>" << phone << "</b>";
                    if ((usage = repository.getServiceNumber(phone.c_str())) != 0)
                        html << " / " << "<a href='/client?id=" << usage->client_account_id << "'>" << usage->client_account_id << "</a>";
                    html << "<br/>\n";
                }
                html << "</td>\n";
            }
        }

        {
            lock_guard<Spinlock> guard(blacklist_trunk->lock);

            if (blacklist_trunk->list_to_add.size() > 0) {
                html << "<td nowrap valign=top>\n";
                html << "BlackListTrunk to Add: <b>" << blacklist_trunk->list_to_add.size() << "</b><br/>\n";
                for (auto phone : blacklist_trunk->list_to_add) {
                    html << "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;";
                    html << "<b>" << phone << "</b>";
                    auto trunk = repository.getTrunkByName(phone.c_str());
                    if (trunk != nullptr) {
                        auto serviceTrunk = repository.getServiceTrunk(trunk->id);
                        if (serviceTrunk != nullptr)
                            html << " / " << "<a href='/client?id=" << serviceTrunk->client_account_id << "'>" << serviceTrunk->client_account_id << "</a>";
                    }
                    html << "<br/>\n";
                }
                html << "</td>\n";
            }

            if (blacklist_trunk->list_to_del.size() > 0) {
                html << "<td nowrap valign=top>\n";
                html << "BlackListTrunk to Del: <b>" << blacklist_trunk->list_to_del.size() << "</b><br/>\n";
                for (auto phone : blacklist_trunk->list_to_del) {
                    html << "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;";
                    html << "<b>" << phone << "</b>";
                    auto trunk = repository.getTrunkByName(phone.c_str());
                    if (trunk != nullptr) {
                        auto serviceTrunk = repository.getServiceTrunk(trunk->id);
                        if (serviceTrunk != nullptr)
                            html << " / " << "<a href='/client?id=" << serviceTrunk->client_account_id << "'>" << serviceTrunk->client_account_id << "</a>";
                    }
                    html << "<br/>\n";
                }
                html << "</td>\n";
            }
        }

        {
            lock_guard<Spinlock> guard(blacklist_global->lock);

            if (blacklist_anti_fraud_disable->list_to_add.size() > 0) {
                html << "<td nowrap valign=top>\n";
                html << "BlackListAntiFraudDisable to Add: <b>" << blacklist_anti_fraud_disable->list_to_add.size() << "</b><br/>\n";
                for (auto phone : blacklist_anti_fraud_disable->list_to_add) {
                    html << "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;";
                    html << "<b>" << phone << "</b>";
                    if ((usage = repository.getServiceNumber(phone.c_str())) != 0)
                        html << " / " << "<a href='/client?id=" << usage->client_account_id << "'>" << usage->client_account_id << "</a>";
                    html << "<br/>\n";
                }
                html << "</td>\n";
            }

            if (blacklist_anti_fraud_disable->list_to_del.size() > 0) {
                html << "<td nowrap valign=top>\n";
                html << "BlackListAntiFraudDisable to Del: <b>" << blacklist_anti_fraud_disable->list_to_del.size() << "</b><br/>\n";
                for (auto phone : blacklist_global->list_to_del) {
                    html << "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;";
                    html << "<b>" << phone << "</b>";
                    if ((usage = repository.getServiceNumber(phone.c_str())) != 0)
                        html << " / " << "<a href='/client?id=" << usage->client_account_id << "'>" << usage->client_account_id << "</a>";
                    html << "<br/>\n";
                }
                html << "</td>\n";
            }
        }

        {
            lock_guard<Spinlock> guard(blacklist_local->lock);

            if (blacklist_local->blacklist.size() > 0) {
                html << "<td nowrap valign=top>\n";
                html << "BlackListLocal: <b>" << blacklist_local->blacklist.size() << "</b><br/>\n";
                for (auto phone : blacklist_local->blacklist) {
                    html << "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;";
                    html << "<b>" << phone << "</b>";
                    if ((usage = repository.getServiceNumber(phone.c_str())) != 0)
                        html << " / " << "<a href='/client?id=" << usage->client_account_id << "'>" << usage->client_account_id << "</a>";
                    html << "<br/>\n";
                }
                html << "</td>\n";
            }
        }

        {
            lock_guard<Spinlock> guard(blacklist_global->lock);

            if (blacklist_global->blacklist.size() > 0) {
                html << "<td nowrap valign=top>\n";
                html << "BlackListGlobal: <b>" << blacklist_global->blacklist.size() << "</b><br/>\n";
                for (auto phone : blacklist_global->blacklist) {
                    html << "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;";
                    if ((usage = repository.getServiceNumber(phone.c_str())) != 0) {
                        html << "<b>" << phone << "</b>";
                        html << " / " << "<a href='/client?id=" << usage->client_account_id << "'>" << usage->client_account_id << "</a>";
                    } else if ((usage = repository.getServiceNumberByTechnicalNumber(atoll(phone.c_str()))) != 0) {
                        html << "<b>" << usage->did << " (" << phone << ")" << "</b>";
                        html << " / " << "<a href='/client?id=" << usage->client_account_id << "'>" << usage->client_account_id << "</a>";
                    } else {
                        html << "<b>" << phone << "</b>";
                    }

                    html << "<br/>\n";
                }
                html << "</td>\n";
            }
        }

        {
            lock_guard<Spinlock> guard(blacklist_trunk->lock);

            if (blacklist_trunk->blacklist.size() > 0) {
                html << "<td nowrap valign=top>\n";
                html << "BlackListTrunk: <b>" << blacklist_trunk->blacklist.size() << "</b><br/>\n";
                for (auto phone : blacklist_trunk->blacklist) {
                    html << "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;";
                    html << "<b>" << phone << "</b>";
                    auto trunk = repository.getTrunkByName(phone.c_str());
                    if (trunk != nullptr) {
                        auto serviceTrunk = repository.getServiceTrunk(trunk->id);
                        if (serviceTrunk != nullptr)
                            html << " / " << "<a href='/client?id=" << serviceTrunk->client_account_id << "'>" << serviceTrunk->client_account_id << "</a>";
                    }
                    html << "<br/>\n";
                }
                html << "</td>\n";
            }
        }

        {
            lock_guard<Spinlock> guard(blacklist_global->lock);

            if (blacklist_anti_fraud_disable->blacklist.size() > 0) {
                html << "<td nowrap valign=top>\n";
                html << "BlackListAntiFraudDisable: <b>" << blacklist_anti_fraud_disable->blacklist.size() << "</b><br/>\n";
                for (auto phone : blacklist_anti_fraud_disable->blacklist) {
                    html << "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;";
                    html << "<b>" << phone << "</b>";
                    if ((usage = repository.getServiceNumber(phone.c_str())) != 0)
                        html << " / " << "<a href='/client?id=" << usage->client_account_id << "'>" << usage->client_account_id << "</a>";
                    html << "<br/>\n";
                }
                html << "</td>\n";
            }
        }

        html << "</tr>\n";
        html << "</table>\n";


    }
};
