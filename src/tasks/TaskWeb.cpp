#include "TaskWeb.h"

#include "../http/server.hpp"
#include "../http/file_handler.hpp"

#include "../common.h"
#include "Task.h"
#include "TaskCurrentCalls.h"

#include "../classes/DataLoader.h"
#include "../classes/CalcFull.h"
#include "../classes/BlackListLocal.h"
#include "../classes/BlackListGlobal.h"


void TaskWeb::operator()()
{
    if (app.conf.web_port == 0) return;

    boost::asio::io_service io_service;
    http::server4::server(io_service, "0.0.0.0",
                          string_fmt("%d",app.conf.web_port),
                          http::server4::file_handler())();
    io_service.run();

}

void TaskWeb::handlerHeader(stringstream &html)
{
    html << "<style>\n";
    html << "* {font-family:monospace !important;}\n";
    html << "td { text-align: center }\n";
    html << "</style>\n";

    html << " <a href='/'>home</a> ";
    html << " <a href='/config'>config</a> ";
    html << string_time(time(NULL));
    html << " <hr/> ";
}

void TaskWeb::handlerHome(stringstream &html)
{
    handlerHeader(html);

    app.tasks_mutex.lock();
    for(list<Task*>::iterator i = app.tasks.begin(); i != app.tasks.end(); i++)
    {
        Task * task = *i;
        task->html(html);
        html << "<hr>\n";
    }
    app.tasks_mutex.unlock();
}

void TaskWeb::handlerConfig(stringstream &html)
{
    handlerHeader(html);

    html << "<h2>Config</h2>\n";
    html << "<hr>\n";
    html << "config_file: " << app.conf.config_file << "<br/>\n";
    html << "log_file: " << app.conf.log_file << "<br/>\n";
    html << "err_log_file: " << app.conf.err_log_file << "<br/>\n";
    html << "pid_file: " << app.conf.pid_file << "<br/>\n";
    html << "<br/>\n";
    html << "main.web_port: " <<  app.conf.web_port << "<br/>\n";
    html << "main.test_mode: " <<  app.conf.test_mode << "<br/>\n";
    html << "<br/>\n";
    html << "db.main: " << app.conf.db_main << "<br/>\n";
    html << "db.rad: " << app.conf.db_rad << "<br/>\n";
    html << "db.calls: " << app.conf.db_calls << "<br/>\n";
    html << "<br/>\n";
    html << "udp.host: " << app.conf.udp_host << "<br/>\n";
    html << "udp.port: " << app.conf.udp_port << "<br/>\n";
    html << "<br/>\n";
    html << "billing.free_seconds: " << app.conf.billing_free_seconds << "<br/>\n";
    html << "billing.dc_break: " << app.conf.billing_dc_break << "<br/>\n";
}

bool TaskWeb::handlerTask(stringstream &html, map<string,string> &parameters)
{
    string task_id;
    if (parameters.find("id") != parameters.end())
        task_id = parameters["id"];
    Task * task = 0;
    app.tasks_mutex.lock();
    for(list<Task*>::iterator i = app.tasks.begin(); i != app.tasks.end(); i++)
    {
        task = *i;
        if (task->id == task_id)
            break;
        else
            task = 0;
    }
    app.tasks_mutex.unlock();

    if (task == 0){
        return false;
    }

    handlerHeader(html);

    task->htmlfull(html);

    return true;
}

void TaskWeb::handlerCounters(stringstream &html)
{
    handlerHeader(html);

    DataLoader *loader = DataLoader::instance();
    loader->rwlock.lock();
    shared_ptr<ClientObjList> c = loader->client;
    loader->rwlock.unlock();

    loader->counter_rwlock.lock();
    shared_ptr<ClientCounter> cl = loader->counter_client;
    if (cl == 0)
    {
        loader->counter_rwlock.unlock();
        return;
    }
    html << "<table><tr><th>client</th><th>month</th><th>month sum</th><th>day</th><th>day sum</th><th>date</th><th>sum</th></tr>";
    map<int, ClientCounterObj>::iterator i;
    for (i = cl->counter.begin(); i != cl->counter.end(); ++i){
        int key = i->first;
        ClientCounterObj value = i->second;
        double sum_month = value.sumMonth();
        double sum_day = value.sumDay();
        double sum_balance = value.sumBalance();

        time_t amount_date = 0;
        if (c != 0)
        {
            pClientObj client = c->find(value.client_id);
            if (client != 0)
            {
                amount_date = client->amount_date;
            }
        }

        if (sum_month == 0 && sum_day == 0 && sum_balance == 0) continue;
        html << "<tr><td>"
                    << key << "</td><td>"
                    << string_date(value.amount_month) << "</td><td>"
                    << sum_month << "</td><td>"
                    << string_date(value.amount_day) << "</td><td>"
                    << sum_day << "</td><td>"
                    << string_time(amount_date) << "</td><td>"
                    << sum_balance << "</td></tr>";
    }
    html << "</table>";
    loader->counter_rwlock.unlock();
}



void TaskWeb::handlerClient(stringstream &html, map<string,string> &parameters)
{
    handlerHeader(html);

    int client_id = atoi(parameters["id"].c_str());

    DataLoader *loader = DataLoader::instance();

    loader->rwlock.lock();
    shared_ptr<ClientObjList> clients = loader->client;
    if (clients == 0)
    {
        loader->rwlock.unlock();
        return;
    }
    shared_ptr<UsageObjList> usages = loader->usage.get(get_tday());
    if (usages == 0)
    {
        loader->rwlock.unlock();
        return;
    }
    pClientObj p_client = clients->find(client_id);
    if (p_client == 0)
    {
        loader->rwlock.unlock();
        html << "Client " << client_id << " not found";
        return;
    }
    loader->rwlock.unlock();

    ClientObj client = *p_client;

    double sum_month, sum_day, sum_balance;
    bool client_disabled_global, client_disabled_local;
    {
        loader->counter_rwlock.lock(); //.lockForRead();
        shared_ptr<ClientCounter> clients_counters = loader->counter_client;
        ClientCounterObj &client_counter = clients_counters->get(client_id);
        sum_month = client_counter.sumMonth();
        sum_day = client_counter.sumDay();
        sum_balance = client_counter.sumBalance();
        client_disabled_global = client_counter.disabled_global;
        client_disabled_local = client_counter.disabled_local;
        loader->counter_rwlock.unlock();
    }

    shared_ptr<CurrentCallsObjList> current_calls_list = TaskCurrentCalls::getList();
    CurrentCallsObjList * calls_list = current_calls_list.get();

    CalcFull calculator;
    calculator.calc_limit(calls_list);

    ClientCounterObj &c2 = calculator.client_counter2->get(client_id);
    sum_balance = sum_balance + c2.sumBalance();
    sum_day = sum_day + c2.sumDay();
    sum_month = sum_month + c2.sumMonth();

    html << "Client Id: <b>" << client.id << "</b><br/>\n";
    html << "<br/>\n";

    if (client_disabled_local)
        html << "Client disabled LOCAL<br/>\n";

    if (client_disabled_global)
        html << "Client disabled GLOBAL<br/>\n";

    html << "<br/>\n";

    BlackList *bl;
    bl = BlackListLocal::instance();
    bl->lock.lock();
    {
        map<long long int,time_t>::iterator i = bl->blacklist.begin();
        while (i != bl->blacklist.end()) {
            pUsageObj usage = usages->find(i->first);
            if (usage != 0 && usage->client_id == client_id)
            {
                html << "Locked Local <b>number</b>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<b>";
                html << lexical_cast<string>(i->first);
                html << "</b><br/>\n";
            }
            ++i;
        }
    }
    bl->lock.unlock();

    bl = BlackListGlobal::instance();
    bl->lock.lock();
    {
        map<long long int,time_t>::iterator i = bl->blacklist.begin();
        while (i != bl->blacklist.end()) {
            pUsageObj usage = usages->find(i->first);
            if (usage != 0 && usage->client_id == client_id)
            {
                html << "Locked Gobal <b>number</b>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<b>";
                html << lexical_cast<string>(i->first);
                html << "</b><br/>\n";
            }
            ++i;
        }
    }
    bl->lock.unlock();

    html << "<br/>\n";

    if (client.credit >= 0)
        html << "Balance avaiable: <b>" << string_fmt("%.2f",client.balance + client.credit - sum_balance) << "</b><br/>\n";

    if (client.limit_d != 0)
        html << "Daily avaiable: <b>" << string_fmt("%.2f",client.limit_d - sum_day) << "</b><br/>\n";

    if (client.limit_m != 0)
        html << "Monthly avaiable: <b>" << string_fmt("%.2f",client.limit_m - sum_month) << "</b><br/>\n";

    html << "<br/>\n";

    html << "Balance accounts: <b>" << string_fmt("%.2f",client.balance) << "</b><br/>\n";
    if (client.limit_d != 0)
        html << "Daily limit: <b>" << client.limit_d << "</b><br/>\n";
    if (client.limit_m != 0)
        html << "Monthly limit: <b>" << client.limit_m << "</b><br/>\n";
    if (client.credit >= 0)
        html << "Credit limit: <b>" << client.credit << "</b><br/>\n";
    if (client.disabled)
        html << "Voip disabled: <b>" << "true" << "</b><br/>\n";
    html << "<br/>\n";

    html << "Last account date: <b>" << string_time(client.amount_date) << "</b><br/>\n";
    html << "Last payed month: <b>" << string_date(client.last_payed_month) << "</b><br/>\n";

    html << "<br/>\n";

    html << "Sum from account: <b>" << string_fmt("%.2f",sum_balance) << "</b><br/>\n";
    html << "Sum Day: <b>" << string_fmt("%.2f",sum_day) << "</b><br/>\n";
    html << "Sum Month: <b>" << string_fmt("%.2f",sum_month) << "</b><br/>\n";
    html << "<br/>\n";


    int calls_count = 0;
    for(int i=0; i<calls_list->count;i++){
        pCallObj call = calls_list->get(i);
        if (call->client_id != client_id) continue;

        calls_count++;
    }

    html << "Current calls: <b>" << calls_count << "</b><br/>\n";
    html << "<br/>\n";

    if (calls_count > 0)
    {
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
        for(int i=0; i<calls_list->count;i++){
            pCallObj call = calls_list->get(i);
            if (call->client_id != client_id) continue;

            html << "<tr>";
            html << "<td>" << call->time << "</td>";
            html << "<td>" << call->usage << "</td>";
            html << "<td><b>" << (call->out ? "out" : "in") << "</b></td>";
            html << "<td>" << call->phone << "</td>";
            html << "<td><b>" << call->len << "</b></td>";
            html << "<td><b>" << string_fmt("%.2f",call->amount/100.0) << "</b></td>";
            html << "<td>" << string_fmt("%.2f",call->price/10000.0) << "</td>";
            html << "<td>" << call->pricelist_id << "</td>";
            html << "<td>" << call->operator_id << "</td>";
            html << "<td>" << string_fmt("%.2f",call->amount_op/100.0) << "</td>";
            html << "<td>" << string_fmt("%.2f",call->price_op/10000.0) << "</td>";
            html << "<td>" << call->geo_id << "</td>";
            html << "</tr>\n";
        }
        html << "</table>\n";
    }

}


