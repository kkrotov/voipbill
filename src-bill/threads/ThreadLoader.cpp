#include "ThreadLoader.h"
#include "../classes/AppBill.h"

bool ThreadLoader::ready() {
    return app().init_sync_done;
}

bool ThreadLoader::prepare() {

    if (!app().init_load_counters_done) {
        Log::info("Loading counters...");
        if (!this->do_load_counters()) {
            return false;
        }
        app().init_load_counters_done = true;
    }

    if (!app().init_load_data_done) {
        Log::info("Loading data...");
        if (!this->do_load_data()) {
            return false;
        }
        app().init_load_data_done = true;
    }

    return true;
}

void ThreadLoader::run() {

    string event;

    try {
        BDbResult res = db_calls.query("SELECT event from event.queue");
        while (res.next()) {

            event = res.get_s(0);

            time_t tday = get_tday();

            db_calls.exec("DELETE from event.queue WHERE event='" + event + "'");


            if (event == "clients") {
                data->client.load(&db_calls);

                {
                    lock_guard<mutex> lock(loader->counter_rwlock);

                    shared_ptr<ClientCounter> cc = loader->counter_client;
                    if (cc != 0) cc->reload(&db_calls);
                }

            } else if (event == "airp") {

                data->airp.load(&db_calls);

            } else if (event == "number") {

                data->number.load(&db_calls);

            } else if (event == "outcome") {

                data->outcome.load(&db_calls);

            } else if (event == "prefixlist") {

                data->prefixlist.load(&db_calls);

            } else if (event == "prefixlist_prefix") {

                data->prefixlistPrefix.load(&db_calls);

            } else if (event == "release_reason") {

                data->releaseReason.load(&db_calls);

            } else if (event == "route_case") {

                data->routeCase.load(&db_calls);

            } else if (event == "route_table") {

                data->routeTable.load(&db_calls);

            } else if (event == "route_table") {

                data->routeTable.load(&db_calls);

            } else if (event == "route_table") {

                data->routeTable.load(&db_calls);

            } else if (event == "route_table_route") {

                data->routeTableRoute.load(&db_calls);

            } else if (event == "routing_report_data") {

                //

            } else if (event == "trunk") {

                data->trunk.load(&db_calls);

            } else if (event == "trunk_number_preprocessing") {

                data->trunkNumberPreprocessing.load(&db_calls);

            } else if (event == "trunk_priority") {

                data->trunkPriority.load(&db_calls);

            } else if (event == "trunk_rule") {

                data->trunkRule.load(&db_calls);

            } else if (event == "defs") {

                data->pricelistPrice.load(&db_calls);

            } else if (event == "instance_settings") {

                data->instanceSettings.load(&db_calls);

            } else if (event == "network_prefix") {

                data->networkPrefix.load(&db_calls);

            } else if (event == "operator") {

                data->voipOperator.load(&db_calls);

            } else if (event == "pricelist") {

                data->pricelist.load(&db_calls);

            } else if (event == "service_number") {

                data->serviceNumber.load(&db_calls);

            } else if (event == "service_trunk") {

                data->serviceTrunk.load(&db_calls);

            } else if (event == "service_trunk_settings") {

                data->serviceTrunkSettings.load(&db_calls);

            } else if (event == "tariff") {

                data->tariff.load(&db_calls);

            } else if (event == "tariff_change_log") {

                data->tariffChangeLog.load(&db_calls);

            } else if (event == "server") {

                data->server.load(&db_calls);

            }

            boost::this_thread::interruption_point();

        }
    } catch (Exception &e) {

        if (event.length() > 0) {
            try {
                db_calls.exec("INSERT INTO event.queue(event)VALUES('" + event + "')");
            } catch (...) {
            }
            event.clear();
        }

        e.addTrace("Loader::run");
        throw e;
    }

}

void ThreadLoader::htmlfull(stringstream &html) {
    this->html(html);

    time_t tday = get_tday();
    time_t tmonth = get_tmonth();

    html << "<table border=1 width=100%>";
    html << "<tr><th></th><th>Updated at</th><th>Size</th><th>Rows</th><th>Last time</th><th>Total time</th></tr>";
    {
        {
            auto dl = &data->server;
            html << "<tr><th>server</th>";
            html << "<td>" << string_time(dl->time()) << "</td><td>" << dl->size() / 1024 << " Kb</td><td>" <<
            dl->rows() << "</td><td>" << dl->timer.sloop() << "</td><td>" << dl->timer.sfull() << "</td>";
            html << "</tr>\n";
        }
        {
            auto dl = &data->instanceSettings;
            html << "<tr><th>instanceSettings</th>";
            html << "<td>" << string_time(dl->time()) << "</td><td>" << dl->size() / 1024 << " Kb</td><td>" <<
            dl->rows() << "</td><td>" << dl->timer.sloop() << "</td><td>" << dl->timer.sfull() << "</td>";
            html << "</tr>\n";
        }
        {
            auto dl = &data->airp;
            html << "<tr><th>airp</th>";
            html << "<td>" << string_time(dl->time()) << "</td><td>" << dl->size() / 1024 << " Kb</td><td>" <<
            dl->rows() << "</td><td>" << dl->timer.sloop() << "</td><td>" << dl->timer.sfull() << "</td>";
            html << "</tr>\n";
        }
        {
            auto dl = &data->number;
            html << "<tr><th>number</th>";
            html << "<td>" << string_time(dl->time()) << "</td><td>" << dl->size() / 1024 << " Kb</td><td>" <<
            dl->rows() << "</td><td>" << dl->timer.sloop() << "</td><td>" << dl->timer.sfull() << "</td>";
            html << "</tr>\n";
        }
        {
            auto dl = &data->outcome;
            html << "<tr><th>outcome</th>";
            html << "<td>" << string_time(dl->time()) << "</td><td>" << dl->size() / 1024 << " Kb</td><td>" <<
            dl->rows() << "</td><td>" << dl->timer.sloop() << "</td><td>" << dl->timer.sfull() << "</td>";
            html << "</tr>\n";
        }
        {
            auto dl = &data->prefixlist;
            html << "<tr><th>prefixlist</th>";
            html << "<td>" << string_time(dl->time()) << "</td><td>" << dl->size() / 1024 << " Kb</td><td>" <<
            dl->rows() << "</td><td>" << dl->timer.sloop() << "</td><td>" << dl->timer.sfull() << "</td>";
            html << "</tr>\n";
        }
        {
            auto dl = &data->prefixlistPrefix;
            html << "<tr><th>prefixlistPrefix</th>";
            html << "<td>" << string_time(dl->time()) << "</td><td>" << dl->size() / 1024 << " Kb</td><td>" <<
            dl->rows() << "</td><td>" << dl->timer.sloop() << "</td><td>" << dl->timer.sfull() << "</td>";
            html << "</tr>\n";
        }
        {
            auto dl = &data->releaseReason;
            html << "<tr><th>releaseReason</th>";
            html << "<td>" << string_time(dl->time()) << "</td><td>" << dl->size() / 1024 << " Kb</td><td>" <<
            dl->rows() << "</td><td>" << dl->timer.sloop() << "</td><td>" << dl->timer.sfull() << "</td>";
            html << "</tr>\n";
        }
        {
            auto dl = &data->routeCase;
            html << "<tr><th>routeCase</th>";
            html << "<td>" << string_time(dl->time()) << "</td><td>" << dl->size() / 1024 << " Kb</td><td>" <<
            dl->rows() << "</td><td>" << dl->timer.sloop() << "</td><td>" << dl->timer.sfull() << "</td>";
            html << "</tr>\n";
        }
        {
            auto dl = &data->routeTable;
            html << "<tr><th>routeTable</th>";
            html << "<td>" << string_time(dl->time()) << "</td><td>" << dl->size() / 1024 << " Kb</td><td>" <<
            dl->rows() << "</td><td>" << dl->timer.sloop() << "</td><td>" << dl->timer.sfull() << "</td>";
            html << "</tr>\n";
        }
        {
            auto dl = &data->routeTableRoute;
            html << "<tr><th>routeTableRoute</th>";
            html << "<td>" << string_time(dl->time()) << "</td><td>" << dl->size() / 1024 << " Kb</td><td>" <<
            dl->rows() << "</td><td>" << dl->timer.sloop() << "</td><td>" << dl->timer.sfull() << "</td>";
            html << "</tr>\n";
        }
        {
            auto dl = &data->trunk;
            html << "<tr><th>trunk</th>";
            html << "<td>" << string_time(dl->time()) << "</td><td>" << dl->size() / 1024 << " Kb</td><td>" <<
            dl->rows() << "</td><td>" << dl->timer.sloop() << "</td><td>" << dl->timer.sfull() << "</td>";
            html << "</tr>\n";
        }
        {
            auto dl = &data->trunkNumberPreprocessing;
            html << "<tr><th>trunkNumberPreprocessing</th>";
            html << "<td>" << string_time(dl->time()) << "</td><td>" << dl->size() / 1024 << " Kb</td><td>" <<
            dl->rows() << "</td><td>" << dl->timer.sloop() << "</td><td>" << dl->timer.sfull() << "</td>";
            html << "</tr>\n";
        }
        {
            auto dl = &data->trunkPriority;
            html << "<tr><th>trunkPriority</th>";
            html << "<td>" << string_time(dl->time()) << "</td><td>" << dl->size() / 1024 << " Kb</td><td>" <<
            dl->rows() << "</td><td>" << dl->timer.sloop() << "</td><td>" << dl->timer.sfull() << "</td>";
            html << "</tr>\n";
        }
        {
            auto dl = &data->trunkRule;
            html << "<tr><th>trunkRule</th>";
            html << "<td>" << string_time(dl->time()) << "</td><td>" << dl->size() / 1024 << " Kb</td><td>" <<
            dl->rows() << "</td><td>" << dl->timer.sloop() << "</td><td>" << dl->timer.sfull() << "</td>";
            html << "</tr>\n";
        }
        {
            auto dl = &data->client;
            html << "<tr><th>client</th>";
            html << "<td>" << string_time(dl->time()) << "</td><td>" << dl->size() / 1024 << " Kb</td><td>" <<
            dl->rows() << "</td><td>" << dl->timer.sloop() << "</td><td>" << dl->timer.sfull() << "</td>";
            html << "</tr>\n";
        }
        {
            auto dl = &data->voipOperator;
            html << "<tr><th>voipOperator</th>";
            html << "<td>" << string_time(dl->time()) << "</td><td>" << dl->size() / 1024 << " Kb</td><td>" <<
            dl->rows() << "</td><td>" << dl->timer.sloop() << "</td><td>" << dl->timer.sfull() << "</td>";
            html << "</tr>\n";
        }
        {
            auto dl = &data->pricelist;
            html << "<tr><th>pricelist</th>";
            html << "<td>" << string_time(dl->time()) << "</td><td>" << dl->size() / 1024 << " Kb</td><td>" <<
            dl->rows() << "</td><td>" << dl->timer.sloop() << "</td><td>" << dl->timer.sfull() << "</td>";
            html << "</tr>\n";
        }
        {
            auto dl = &data->pricelistPrice;
            html << "<tr><th>pricelistPrice</th>";
            html << "<td>" << string_time(dl->time()) << "</td><td>" << dl->size() / 1024 << " Kb</td><td>" <<
            dl->rows() << "</td><td>" << dl->timer.sloop() << "</td><td>" << dl->timer.sfull() << "</td>";
            html << "</tr>\n";
        }
        {
            auto dl = &data->networkPrefix;
            html << "<tr><th>networkPrefix</th>";
            html << "<td>" << string_time(dl->time()) << "</td><td>" << dl->size() / 1024 << " Kb</td><td>" <<
            dl->rows() << "</td><td>" << dl->timer.sloop() << "</td><td>" << dl->timer.sfull() << "</td>";
            html << "</tr>\n";
        }
        {
            auto dl = &data->geoPrefix;
            html << "<tr><th>geoPrefix</th>";
            html << "<td>" << string_time(dl->time()) << "</td><td>" << dl->size() / 1024 << " Kb</td><td>" <<
            dl->rows() << "</td><td>" << dl->timer.sloop() << "</td><td>" << dl->timer.sfull() << "</td>";
            html << "</tr>\n";
        }
        {
            auto dl = &data->serviceNumber;
            html << "<tr><th>serviceNumber</th>";
            html << "<td>" << string_time(dl->time()) << "</td><td>" << dl->size() / 1024 << " Kb</td><td>" <<
            dl->rows() << "</td><td>" << dl->timer.sloop() << "</td><td>" << dl->timer.sfull() << "</td>";
            html << "</tr>\n";
        }
        {
            auto dl = &data->serviceTrunk;
            html << "<tr><th>serviceTrunk</th>";
            html << "<td>" << string_time(dl->time()) << "</td><td>" << dl->size() / 1024 << " Kb</td><td>" <<
            dl->rows() << "</td><td>" << dl->timer.sloop() << "</td><td>" << dl->timer.sfull() << "</td>";
            html << "</tr>\n";
        }
        {
            auto dl = &data->serviceTrunkSettings;
            html << "<tr><th>serviceTrunkSettings</th>";
            html << "<td>" << string_time(dl->time()) << "</td><td>" << dl->size() / 1024 << " Kb</td><td>" <<
            dl->rows() << "</td><td>" << dl->timer.sloop() << "</td><td>" << dl->timer.sfull() << "</td>";
            html << "</tr>\n";
        }
        {
            auto dl = &data->tariff;
            html << "<tr><th>tariff</th>";
            html << "<td>" << string_time(dl->time()) << "</td><td>" << dl->size() / 1024 << " Kb</td><td>" <<
            dl->rows() << "</td><td>" << dl->timer.sloop() << "</td><td>" << dl->timer.sfull() << "</td>";
            html << "</tr>\n";
        }
        {
            auto dl = &data->tariffChangeLog;
            html << "<tr><th>tariffChangeLog</th>";
            html << "<td>" << string_time(dl->time()) << "</td><td>" << dl->size() / 1024 << " Kb</td><td>" <<
            dl->rows() << "</td><td>" << dl->timer.sloop() << "</td><td>" << dl->timer.sfull() << "</td>";
            html << "</tr>\n";
        }
        {
            auto dl = &data->globalCounters;
            html << "<tr><th>globalCounters</th>";
            html << "<td>" << string_time(dl->time()) << "</td><td>" << dl->size() / 1024 << " Kb</td><td>" <<
            dl->rows() << "</td><td>" << dl->timer.sloop() << "</td><td>" << dl->timer.sfull() << "</td>";
            html << "</tr>\n";
        }
        {
            auto dl = &data->currentCdrData;
            html << "<tr><th>currentCdrData</th>";
            html << "<td>" << string_time(dl->time()) << "</td><td>" << dl->size() / 1024 << " Kb</td><td>" <<
            dl->rows() << "</td><td>" << dl->timer.sloop() << "</td><td>" << dl->timer.sfull() << "</td>";
            html << "</tr>\n";
        }
    }
    {
        lock_guard<mutex> lock(loader->counter_rwlock);

        {
            auto ol = loader->counter_client.get();
            html << "<tr><th>Client counter</th>";
            if (ol != 0) {
                html << "<td>" << string_time(ol->loadTime()) << "</td><td></td><td>" <<
                ol->counter.size() << "</td><td>" << ol->t.sloop() << "</td><td>" <<
                ol->t.sfull() << "</td>";
            } else {
                html << "<td colspan=5></td>";
            }
            html << "</tr>\n";
        }
        {
            auto ol = loader->counter_fmin.get(tmonth).get();
            html << "<tr><th>Fmin counter</th>";
            if (ol != 0) {
                html << "<td>" << string_time(ol->loadTime()) << "</td><td></td><td>" <<
                ol->counter.size() << "</td><td>" << ol->t.sloop() << "</td><td>" << ol->t.sfull() <<
                "</td>";
            } else {
                html << "<td colspan=5></td>";
            }
            html << "</tr>\n";
        }
    }

    html << "</table>\n";

}

bool ThreadLoader::do_load_data(BDb *db, DataLoader *loader, DataContainer *data) {
    bool success = true;

    if (loader == 0) loader = this->loader;
    if (data == 0) data = this->data;
    if (db == 0) db = &db_calls;


    try {
        db_calls.exec("DELETE from event.queue");

        data->loadAll(db);

    } catch (Exception &e) {
        e.addTrace("Loader::do_load_data");
        Log::exception(e);
        success = false;
    }

    return success;
}

bool ThreadLoader::do_load_counters(BDb *db, DataLoader *loader) {
    bool success = true;

    if (loader == 0) loader = this->loader;
    if (db == 0) db = &db_calls;

    ClientCounter * counter_client = new ClientCounter();
    FminCounter * counter_fmin = new FminCounter();
    time_t t_month = get_tmonth();
    try {

        counter_client->load(db, 0);

        counter_fmin->load(db, t_month);

    } catch (Exception &e) {
        e.addTrace("Loader::do_load_counters");
        Log::exception(e);
        success = false;
    }

    {
        lock_guard<mutex> lock(loader->counter_rwlock);

        if (counter_client->loaded)
            loader->counter_client = shared_ptr<ClientCounter>(counter_client);
        else
            delete counter_client;

        if (counter_fmin->loaded)
            loader->counter_fmin.addlist(t_month, counter_fmin);
        else
            delete counter_fmin;
    }

    return success;
}

ThreadLoader::ThreadLoader() {
    id = idName();
    name = "Loader";

    loader = DataLoader::instance();
    data = DataContainer::instance();
    db_calls.setCS(app().conf.db_calls);
}
