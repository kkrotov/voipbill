#include "ThreadWeb.h"

#include "../../src/http/server.hpp"
#include "../http/file_handler.hpp"

#include "../../src/common.h"
#include "../../src/threads/Thread.h"
#include "ThreadSelectCurrentCalls.h"
#include "ThreadSelectGlobalCounters.h"
#include "../classes/AppBill.h"
#include "../classes/DataLoader.h"
#include "../classes/CalcFull.h"
#include "../classes/BlackListLocal.h"
#include "../classes/BlackListGlobal.h"
#include "../classes/BlackListTrunk.h"

void ThreadWeb::operator()() {
    if (app().conf.web_port == 0) return;

    http::server4::server(io_service, "0.0.0.0",
            string_fmt("%d", app().conf.web_port),
            http::server4::file_handler())();
    io_service.run();
}

void ThreadWeb::stop() {
    io_service.stop();
}

void ThreadWeb::handlerHeader(stringstream &html) {
    html << "<style>\n";
    html << "* {font-family:monospace !important;}\n";
    html << "td { text-align: center }\n";
    html << "</style>\n";

    html << " <a href='/'>home</a> ";
    html << " <a href='/config'>config</a> ";
    html << string_time(time(NULL)) << " ";

    AppStatus status = app().getStatus();
    AppStatus real_status = app().getRealStatus();
    html << "<b>" << AppStatusNames[real_status] << "</b>";
    if (status != real_status) {
        html << " (" << AppStatusNames[status] << ")";
    }

    html << " <hr/> ";
}

void ThreadWeb::handlerHome(stringstream &html) {
    handlerHeader(html);
    
    app().threads.forAllThreads([&](Thread* thread) {
        thread->html(html);
        html << "<hr>\n";
        return true;
    });
}

void ThreadWeb::handlerConfig(stringstream &html) {
    handlerHeader(html);

    html << "<h2>Config</h2>\n";
    html << "build date: <b>" << build_date() << " " << build_time() << "</b><br/>\n";
    html << "config_file: " << app().conf.config_file << "<br/>\n";
    html << "pid_file: " << app().conf.pid_file << "<br/>\n";
    html << "<hr>\n";
    html << "log_file_filename: " << app().conf.log_file_filename << "<br/>\n";
    html << "log_file_min_level: " << app().conf.log_file_min_level << "<br/>\n";
    html << "log_file_max_level: " << app().conf.log_file_max_level << "<br/>\n";
    html << "log_file2_filename: " << app().conf.log_file2_filename << "<br/>\n";
    html << "log_file2_min_level: " << app().conf.log_file2_min_level << "<br/>\n";
    html << "log_file2_max_level: " << app().conf.log_file2_max_level << "<br/>\n";
    html << "log_syslog_ident: " << app().conf.log_syslog_ident << "<br/>\n";
    html << "log_syslog_min_level: " << app().conf.log_syslog_min_level << "<br/>\n";
    html << "log_syslog_max_level: " << app().conf.log_syslog_max_level << "<br/>\n";

    html << "<br/>\n";
    html << "main.web_port: " << app().conf.web_port << "<br/>\n";
    html << "main.api_port: " << app().conf.api_port << "<br/>\n";
    html << "main.test_mode: " << app().conf.test_mode << "<br/>\n";
    html << "<br/>\n";
    html << "log.grouping_interval: " << app().conf.log_grouping_interval << "<br/>\n";
    html << "<br/>\n";
    html << "db.main: " << app().conf.db_main << "<br/>\n";
    html << "db.rad: " << app().conf.db_rad << "<br/>\n";
    html << "db.calls: " << app().conf.db_calls << "<br/>\n";
    html << "<br/>\n";
    html << "udp.host: " << app().conf.openca_udp_host << "<br/>\n";
    html << "udp.port: " << app().conf.openca_udp_port << "<br/>\n";
    html << "<br/>\n";
    html << "billing.free_seconds: " << app().conf.billing_free_seconds << "<br/>\n";
    html << "billing.dc_break: " << app().conf.billing_dc_break << "<br/>\n";
    html << "billing.global_counters_select_interval: " << app().conf.global_counters_select_interval << "<br/>\n";
}

bool ThreadWeb::handlerTask(stringstream &html, map<string, string> &parameters) {
    string task_id;
    if (parameters.find("id") != parameters.end())
        task_id = parameters["id"];
    Thread * thread = 0;

    app().threads.forAllThreads([&](Thread* th) {
        if (th->id == task_id) {
            thread = th;
            return false;
        }
        return true;
    });

    if (thread == 0) {
        return false;
    }

    handlerHeader(html);

    thread->htmlfull(html);

    return true;
}

void ThreadWeb::handlerCounters(stringstream &html) {
    handlerHeader(html);

    DataLoader *loader = DataLoader::instance();
    shared_ptr<ClientObjList> c;

    {
        lock_guard<mutex> lock(loader->rwlock);
        c = loader->client;
    }

    lock_guard<mutex> lock(loader->counter_rwlock);
    shared_ptr<ClientCounter> cl = loader->counter_client;
    if (cl == 0) return;

    html << "<table><tr><th>client</th><th>month</th><th>month sum</th><th>day</th><th>day sum</th><th>date</th><th>sum</th></tr>";
    map<int, ClientCounterObj>::iterator i;
    for (i = cl->counter.begin(); i != cl->counter.end(); ++i) {
        int key = i->first;
        ClientCounterObj value = i->second;
        double sum_month = value.sumMonth();
        double sum_day = value.sumDay();
        double sum_balance = value.sumBalance();

        time_t amount_date = 0;
        if (c != 0) {
            pClientObj client = c->find(value.client_id);
            if (client != 0) {
                amount_date = client->amount_date;
            }
        }

        if (sum_month == 0 && sum_day == 0 && sum_balance == 0) continue;
        html << "<tr><td>"
                << "<a href='/client?id=" << key << "'>" << key << "</a></td><td>"
                << string_date(value.amount_month) << "</td><td>"
                << sum_month << "</td><td>"
                << string_date(value.amount_day) << "</td><td>"
                << sum_day << "</td><td>"
                << string_time(amount_date) << "</td><td>"
                << sum_balance << "</td></tr>";
    }
    html << "</table>";
}

void ThreadWeb::handlerClient(stringstream &html, map<string, string> &parameters) {
    handlerHeader(html);

    int client_id = atoi(parameters["id"].c_str());

    DataLoader *loader = DataLoader::instance();

    shared_ptr<ClientObjList> clients;
    shared_ptr<UsageObjList> usages;
    pClientObj p_client;

    {
        lock_guard<mutex> lock(loader->rwlock);

        clients = loader->client;
        if (clients == 0) return;

        usages = loader->usage.get(get_tday());
        if (usages == 0) return;

        p_client = clients->find(client_id);
        if (p_client == 0) {
            html << "Client " << client_id << " not found";
            return;
        }
    }

    ClientObj client = *p_client;

    double sum_month, sum_day, sum_balance;
    double sum_month2, sum_day2, sum_balance2;
    bool client_disabled_global, client_disabled_local;
    {
        lock_guard<mutex> lock(loader->counter_rwlock);

        shared_ptr<ClientCounter> cl = loader->counter_client;
        if (cl == 0) return;

        ClientCounterObj &client_counter = cl->get(client_id);
        sum_month = client_counter.sumMonth();
        sum_day = client_counter.sumDay();
        sum_balance = client_counter.sumBalance();
        client_disabled_global = client_counter.disabled_global;
        client_disabled_local = client_counter.disabled_local;
    }

    shared_ptr<CurrentCallsObjList> current_calls_list = ThreadSelectCurrentCalls::getList();
    CurrentCallsObjList * calls_list = current_calls_list.get();

    CalcFull calculator;
    calculator.calc_current(calls_list);

    ClientCounterObj &c2 = calculator.client_counter2->get(client_id);
    sum_balance2 = c2.sumBalance();
    sum_day2 = c2.sumDay();
    sum_month2 = c2.sumMonth();
    
    double sum_month_global = 0, sum_day_global = 0, sum_balance_global = 0;
    pGlobalCountersObj globalCounter = ThreadSelectGlobalCounters::getList()->find(client_id);
    if (globalCounter) {
        sum_balance_global += globalCounter->sumBalance();
        sum_day_global += globalCounter->sumDay();
        sum_month_global += globalCounter->sumMonth();
    }


    html << "Client Id: <b>" << client.id << "</b><br/>\n";
    html << "-----<br/>\n";

    if (client_disabled_local)
        html << "Client disabled LOCAL<br/>\n";

    if (client_disabled_global)
        html << "Client disabled GLOBAL<br/>\n";

    html << "-----<br/>\n";


    {
        BlackList *bl = BlackListLocal::instance();

        lock_guard<mutex> lock(bl->rwlock);

        map<long long int, time_t>::iterator i = bl->blacklist.begin();
        while (i != bl->blacklist.end()) {
            pUsageObj usage = usages->find(i->first);
            if (usage != 0 && usage->client_id == client_id) {
                html << "Locked Local <b>number</b>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<b>";
                html << lexical_cast<string>(i->first);
                html << "</b><br/>\n";
            }
            ++i;
        }
    }

    {
        BlackList *bl = BlackListGlobal::instance();

        lock_guard<mutex> lock(bl->rwlock);

        map<long long int, time_t>::iterator i = bl->blacklist.begin();
        while (i != bl->blacklist.end()) {
            pUsageObj usage = usages->find(i->first);
            if (usage != 0 && usage->client_id == client_id) {
                html << "Locked Global <b>number</b>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<b>";
                html << lexical_cast<string>(i->first);
                html << "</b><br/>\n";
            }
            ++i;
        }
    }
    
    {
        BlackList *bl = BlackListTrunk::instance();

        lock_guard<mutex> lock(bl->rwlock);

        map<long long int, time_t>::iterator i = bl->blacklist.begin();
        while (i != bl->blacklist.end()) {
            pUsageObj usage = usages->find(i->first);
            if (usage != 0 && usage->client_id == client_id) {
                html << "Locked Trunk <b>number</b>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<b>";
                html << lexical_cast<string>(i->first);
                html << "</b><br/>\n";
            }
            ++i;
        }
    }

    html << "-----<br/>\n";

    if (client.hasCreditLimit()) {
        html << "Balance avaiable: <b>" << string_fmt("%.2f", client.balance + client.credit - sum_balance - sum_balance2 - sum_balance_global) << "</b> = ";
        html << string_fmt("%.2f", client.balance) << " (balance) + " << string_fmt("%d", client.credit) << " (credit) - " << string_fmt("%.2f", sum_balance) << " (local) - " << string_fmt("%.2f", sum_balance2) << " (current) - " << string_fmt("%.2f", sum_balance_global) << " (global) <br/>\n";
    }

    if (client.hasDailyLimit()) {
        html << "Daily avaiable: <b>" << string_fmt("%.2f", client.limit_d - sum_day - sum_day2 - sum_day_global) << "</b> = ";
        html << string_fmt("%d", client.limit_d) << " (limit_d) - " << string_fmt("%.2f", sum_day) << " (local) - " << string_fmt("%.2f", sum_day2) << " (current) - " << string_fmt("%.2f", sum_day_global) << " (global) <br/>\n";
    }

    if (client.hasMonthlyLimit()) {
        html << "Monthly avaiable: <b>" << string_fmt("%.2f", client.limit_m - sum_month - sum_month2 - sum_month_global) << "</b> = ";
        html << string_fmt("%d", client.limit_m) << " (limit_m) - " << string_fmt("%.2f", sum_month) << " (local) - " << string_fmt("%.2f", sum_month2) << " (current) - " << string_fmt("%.2f", sum_month_global) << " (global) <br/>\n";
    }

    html << "-----<br/>\n";

    if (client.disabled) {
        html << "Voip disabled: <b>" << "true" << "</b><br/>\n";
    }

    html << "-----<br/>\n";

    html << "Last account date: <b>" << string_time(client.amount_date) << "</b><br/>\n";
    html << "Last payed month: <b>" << string_date(client.last_payed_month) << "</b><br/>\n";

    html << "-----<br/>\n";

    html << "Sum from account: <b>" << string_fmt("%.2f", sum_balance + sum_balance2 + sum_balance_global) << "</b> = " << string_fmt("%.2f", sum_balance) << " (local) + " << string_fmt("%.2f", sum_balance2) << " (current) + " << string_fmt("%.2f", sum_balance_global) << " (global) <br/>\n";
    html << "Sum Day: <b>" << string_fmt("%.2f", sum_day + sum_day2 + sum_day_global) << "</b> = " << string_fmt("%.2f", sum_day) << " (local) + " << string_fmt("%.2f", sum_day2) << " (current) + " << string_fmt("%.2f", sum_day_global) << " (global) <br/>\n";
    html << "Sum Month: <b>" << string_fmt("%.2f", sum_month + sum_month2 + sum_month_global) << "</b> = " << string_fmt("%.2f", sum_month) << " (local) + " << string_fmt("%.2f", sum_month2) << " (current) + " << string_fmt("%.2f", sum_month_global) << " (global) <br/>\n";
    
    html << "-----<br/>\n";



    int calls_count = 0;
    for (int i = 0; i < calls_list->count; i++) {
        pCallObj call = calls_list->get(i);
        if (call->client_id != client_id) continue;

        calls_count++;
    }

    html << "Current calls: <b>" << calls_count << "</b><br/>\n";
    html << "<br/>\n";

    if (calls_count > 0) {
        html << "<table width=100% border=1 cellspacing=0>\n";
        html << "<tr>";
        html << "<th>time</td>";
        html << "<th>usage_num</td>";
        html << "<th>direction</th>";
        html << "<th>phone_num</th>";
        html << "<th>len</th>";
        html << "<th>amount</th>";
        html << "<th>price</th>";
        html << "<th>pricelist</th>";
        html << "<th>operator</th>";
        html << "<th>amount_op</th>";
        html << "<th>price_op</th>";
        html << "<th>geo_id</th>";
        html << "</tr>\n";
        for (int i = 0; i < calls_list->count; i++) {
            pCallObj call = calls_list->get(i);
            if (call->client_id != client_id) continue;

            html << "<tr>";
            html << "<td>" << call->time << "</td>";
            html << "<td>" << call->usage_num << "</td>";
            html << "<td><b>" << (call->out ? "out" : "in") << "</b></td>";
            html << "<td>" << call->phone_num << "</td>";
            html << "<td><b>" << call->len << "</b></td>";
            html << "<td><b>" << string_fmt("%.2f", call->amount_mcn / 100.0) << "</b></td>";
            html << "<td>" << string_fmt("%.2f", call->price_mcn / 10000.0) << "</td>";
            html << "<td>" << call->pricelist_mcn_id << "</td>";
            html << "<td>" << call->operator_id << "</td>";
            html << "<td>" << string_fmt("%.2f", call->amount_op / 100.0) << "</td>";
            html << "<td>" << string_fmt("%.2f", call->price_op / 10000.0) << "</td>";
            html << "<td>" << call->geo_id << "</td>";
            html << "</tr>\n";
        }
        html << "</table>\n";
    }

}
