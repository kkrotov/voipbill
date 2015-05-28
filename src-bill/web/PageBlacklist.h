#pragma once

#include "BasePage.h"
#include "../data/DataContainer.h"
#include "../classes/BlackListLocal.h"
#include "../classes/BlackListGlobal.h"
#include "../classes/BlackListTrunk.h"

class PageBlacklist : public BasePage {
public:
    bool canHandle(std::string &path) {
        return path == "/blacklist";
    }
    void render(std::stringstream &html, map<string, string> &parameters) {
        renderHeader(html);

        PreparedData preparedData;
        if (!DataContainer::instance()->prepareData(preparedData, time(nullptr))) {
            return;
        }

        auto blacklist_global = BlackListGlobal::instance();
        auto blacklist_local = BlackListLocal::instance();
        auto blacklist_trunk = BlackListTrunk::instance();

        html << "<table>\n";
        html << "<tr>\n";

        auto usages = preparedData.serviceNumber;
        ServiceNumber * usage;
        {
            lock_guard<Spinlock> guard(blacklist_local->lock);

            if (blacklist_local->list_to_add.size() > 0) {
                html << "<td nowrap valign=top>\n";
                html << "BlackListLocal to Add: <b>" << blacklist_local->list_to_add.size() << "</b><br/>\n";
                for (auto phone : blacklist_local->list_to_add) {
                    html << "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;";
                    html << "<b>" << phone << "</b>";
                    if ((usage = usages->find(atoll(phone.c_str()), time(nullptr))) != 0)
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
                    if ((usage = usages->find(atoll(phone.c_str()), time(nullptr))) != 0)
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
                    if ((usage = usages->find(atoll(phone.c_str()), time(nullptr))) != 0)
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
                    if ((usage = usages->find(atoll(phone.c_str()), time(nullptr))) != 0)
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
                    auto trunk = preparedData.trunkByName->find(phone.c_str());
                    if (trunk == nullptr) {
                        trunk = preparedData.trunkByAlias->find(phone.c_str());
                    }
                    if (trunk != nullptr) {
                        auto serviceTrunk = preparedData.serviceTrunk->find(trunk->id, time(nullptr));
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
                    auto trunk = preparedData.trunkByName->find(phone.c_str());
                    if (trunk == nullptr) {
                        trunk = preparedData.trunkByAlias->find(phone.c_str());
                    }
                    if (trunk != nullptr) {
                        auto serviceTrunk = preparedData.serviceTrunk->find(trunk->id, time(nullptr));
                        if (serviceTrunk != nullptr)
                            html << " / " << "<a href='/client?id=" << serviceTrunk->client_account_id << "'>" << serviceTrunk->client_account_id << "</a>";
                    }
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
                    if ((usage = usages->find(atoll(phone.c_str()), time(nullptr))) != 0)
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
                    html << "<b>" << phone << "</b>";
                    if ((usage = usages->find(atoll(phone.c_str()), time(nullptr))) != 0)
                        html << " / " << "<a href='/client?id=" << usage->client_account_id << "'>" << usage->client_account_id << "</a>";
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
                    auto trunk = preparedData.trunkByName->find(phone.c_str());
                    if (trunk == nullptr) {
                        trunk = preparedData.trunkByAlias->find(phone.c_str());
                    }
                    if (trunk != nullptr) {
                        auto serviceTrunk = preparedData.serviceTrunk->find(trunk->id, time(nullptr));
                        if (serviceTrunk != nullptr)
                            html << " / " << "<a href='/client?id=" << serviceTrunk->client_account_id << "'>" << serviceTrunk->client_account_id << "</a>";
                    }
                    html << "<br/>\n";
                }
                html << "</td>\n";
            }
        }

        html << "</tr>\n";
        html << "</table>\n";


    }
};