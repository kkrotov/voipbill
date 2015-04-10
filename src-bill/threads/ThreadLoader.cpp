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
    ServerList * server = 0;
    ClientList * client = 0;
    DestObjList * dest = 0;
    OperatorList * oper = 0;
    PricelistList * pricelist = 0;
    UsageObjList * usage = 0;
    PriceObjList * price = 0;
    NetworkPrefixList * network_prefix = 0;

    try {
        BDbResult res = db_calls.query("SELECT event from event.queue WHERE app='billing'");
        while (res.next()) {

            event = res.get_s(0);

            time_t tday = get_tday();

            db_calls.exec("DELETE from event.queue WHERE app='billing' and event='" + event + "'");


            if (event == "clients") {

                client = new ClientList();
                client->load(&db_calls, 0);

                {
                    lock_guard<mutex> lock(loader->rwlock);

                    shared_ptr<ClientCounter> cc = loader->counter_client;
                    if (cc != 0) cc->reload(&db_calls);
                }

            } else
                if (event == "dest") {

                dest = new DestObjList();
                dest->load(&db_calls, 0);

            } else
                if (event == "operator") {

                oper = new OperatorList();
                oper->load(&db_calls, 0);

            } else
                if (event == "pricelist") {

                pricelist = new PricelistList();
                pricelist->load(&db_calls, 0);

            } else
                if (event == "usage") {

                usage = new UsageObjList();
                usage->load(&db_calls, tday);

            } else
                if (event == "price") {

                price = new PriceObjList();
                price->load(&db_calls, tday);

            } else
                if (event == "network_prefix") {

                network_prefix = new NetworkPrefixList();
                network_prefix->load(&db_calls, tday);

            }

            boost::this_thread::interruption_point();

        }
    } catch (Exception &e) {

        errors++;
        if (event.length() > 0) {
            try {
                db_calls.exec("INSERT INTO event.queue(app,event)VALUES('billing','" + event + "')");
            } catch (...) {
            }
            event.clear();
        }

        e.addTrace("Loader::run");
        throw e;
    }

    if (server != 0 || client != 0 || dest != 0 || oper != 0 || pricelist != 0 || usage != 0 || price != 0 || network_prefix != 0) {
        lock_guard<mutex> lock(loader->rwlock);

        if (server != 0) {
            if (server->loaded) loader->server = shared_ptr<ServerList>(server);
            else delete server;
            server = 0;
        }
        if (client != 0) {
            if (client->loaded) loader->client = shared_ptr<ClientList>(client);
            else delete client;
            client = 0;
        }
        if (dest != 0) {
            if (dest->loaded) loader->dest = shared_ptr<DestObjList>(dest);
            else delete dest;
            dest = 0;
        }
        if (oper != 0) {
            if (oper->loaded) loader->oper = shared_ptr<OperatorList>(oper);
            else delete oper;
            oper = 0;
        }

        if (pricelist != 0) {
            if (pricelist->loaded) loader->pricelist = shared_ptr<PricelistList>(pricelist);
            else delete pricelist;
            pricelist = 0;
        }


        if (usage != 0) {
            if (usage->loaded) {
                loader->usage.addlist(usage->dt, usage);
                loader->usageReloadTimestamp = time(NULL);
            } else {
                delete usage;
            }
            usage = 0;
        }
        if (price != 0) {
            if (price->loaded) loader->price.addlist(price->dt, price);
            else delete price;
            price = 0;
        }
        if (network_prefix != 0) {
            if (network_prefix->loaded) loader->network_prefix.addlist(network_prefix->dt, network_prefix);
            else delete network_prefix;
            network_prefix = 0;
        }
    }

}

void ThreadLoader::htmlfull(stringstream &html) {
    this->html(html);

    html << "Time loop: <b>" << t.sloop() + "</b><br/>\n";
    html << "Time full loop: <b>" << t.sfull() + "</b><br/>\n";
    html << "loops: <b>" << t.count << "</b><br/>\n";
    html << "Loader errors count: <b>" << errors << "</b><br/>\n";
    html << "<br/>\n";

    ObjList * ol;
    BaseData * dl;
    time_t tday = get_tday();
    time_t tmonth = get_tmonth();

    html << "<table border=1 width=100%>";
    html << "<tr><th></th><th>Updated at</th><th>Size</th><th>Rows</th><th>Last time</th><th>Total time</th></tr>";
    {
        lock_guard<mutex> lock(loader->rwlock);

        dl = &data->server;
        html << "<tr><th>server</th>";
        if (dl->ready()) {
            html << "<td>" << string_time(dl->time()) << "</td><td>" <<  dl->size() / 1024 << " Kb</td><td>" <<  dl->rows() << "</td><td>" << dl->timer.sloop() << "</td><td>" << dl->timer.sfull() << "</td>";
        } else {
            html << "<td colspan=5></td>";
        }
        html << "</tr>\n";

        dl = &data->instanceSettings;
        html << "<tr><th>instanceSettings</th>";
        if (dl->ready()) {
            html << "<td>" << string_time(dl->time()) << "</td><td>" <<  dl->size() / 1024 << " Kb</td><td>" <<  dl->rows() << "</td><td>" << dl->timer.sloop() << "</td><td>" << dl->timer.sfull() << "</td>";
        } else {
            html << "<td colspan=5></td>";
        }
        html << "</tr>\n";

        dl = &data->airp;
        html << "<tr><th>airp</th>";
        if (dl->ready()) {
            html << "<td>" << string_time(dl->time()) << "</td><td>" <<  dl->size() / 1024 << " Kb</td><td>" <<  dl->rows() << "</td><td>" << dl->timer.sloop() << "</td><td>" << dl->timer.sfull() << "</td>";
        } else {
            html << "<td colspan=5></td>";
        }
        html << "</tr>\n";

        dl = &data->number;
        html << "<tr><th>number</th>";
        if (dl->ready()) {
            html << "<td>" << string_time(dl->time()) << "</td><td>" <<  dl->size() / 1024 << " Kb</td><td>" <<  dl->rows() << "</td><td>" << dl->timer.sloop() << "</td><td>" << dl->timer.sfull() << "</td>";
        } else {
            html << "<td colspan=5></td>";
        }
        html << "</tr>\n";

        dl = &data->outcome;
        html << "<tr><th>outcome</th>";
        if (dl->ready()) {
            html << "<td>" << string_time(dl->time()) << "</td><td>" <<  dl->size() / 1024 << " Kb</td><td>" <<  dl->rows() << "</td><td>" << dl->timer.sloop() << "</td><td>" << dl->timer.sfull() << "</td>";
        } else {
            html << "<td colspan=5></td>";
        }
        html << "</tr>\n";

        dl = &data->prefixlist;
        html << "<tr><th>prefixlist</th>";
        if (dl->ready()) {
            html << "<td>" << string_time(dl->time()) << "</td><td>" <<  dl->size() / 1024 << " Kb</td><td>" <<  dl->rows() << "</td><td>" << dl->timer.sloop() << "</td><td>" << dl->timer.sfull() << "</td>";
        } else {
            html << "<td colspan=5></td>";
        }
        html << "</tr>\n";

        dl = &data->prefixlistPrefix;
        html << "<tr><th>prefixlistPrefix</th>";
        if (dl->ready()) {
            html << "<td>" << string_time(dl->time()) << "</td><td>" <<  dl->size() / 1024 << " Kb</td><td>" <<  dl->rows() << "</td><td>" << dl->timer.sloop() << "</td><td>" << dl->timer.sfull() << "</td>";
        } else {
            html << "<td colspan=5></td>";
        }
        html << "</tr>\n";

        dl = &data->releaseReason;
        html << "<tr><th>releaseReason</th>";
        if (dl->ready()) {
            html << "<td>" << string_time(dl->time()) << "</td><td>" <<  dl->size() / 1024 << " Kb</td><td>" <<  dl->rows() << "</td><td>" << dl->timer.sloop() << "</td><td>" << dl->timer.sfull() << "</td>";
        } else {
            html << "<td colspan=5></td>";
        }
        html << "</tr>\n";

        dl = &data->routeCase;
        html << "<tr><th>routeCase</th>";
        if (dl->ready()) {
            html << "<td>" << string_time(dl->time()) << "</td><td>" <<  dl->size() / 1024 << " Kb</td><td>" <<  dl->rows() << "</td><td>" << dl->timer.sloop() << "</td><td>" << dl->timer.sfull() << "</td>";
        } else {
            html << "<td colspan=5></td>";
        }
        html << "</tr>\n";

        dl = &data->routeTable;
        html << "<tr><th>routeTable</th>";
        if (dl->ready()) {
            html << "<td>" << string_time(dl->time()) << "</td><td>" <<  dl->size() / 1024 << " Kb</td><td>" <<  dl->rows() << "</td><td>" << dl->timer.sloop() << "</td><td>" << dl->timer.sfull() << "</td>";
        } else {
            html << "<td colspan=5></td>";
        }
        html << "</tr>\n";

        dl = &data->routeTableRoute;
        html << "<tr><th>routeTableRoute</th>";
        if (dl->ready()) {
            html << "<td>" << string_time(dl->time()) << "</td><td>" <<  dl->size() / 1024 << " Kb</td><td>" <<  dl->rows() << "</td><td>" << dl->timer.sloop() << "</td><td>" << dl->timer.sfull() << "</td>";
        } else {
            html << "<td colspan=5></td>";
        }
        html << "</tr>\n";

        dl = &data->trunk;
        html << "<tr><th>trunk</th>";
        if (dl->ready()) {
            html << "<td>" << string_time(dl->time()) << "</td><td>" <<  dl->size() / 1024 << " Kb</td><td>" <<  dl->rows() << "</td><td>" << dl->timer.sloop() << "</td><td>" << dl->timer.sfull() << "</td>";
        } else {
            html << "<td colspan=5></td>";
        }
        html << "</tr>\n";

        dl = &data->trunkNumberPreprocessing;
        html << "<tr><th>trunkNumberPreprocessing</th>";
        if (dl->ready()) {
            html << "<td>" << string_time(dl->time()) << "</td><td>" <<  dl->size() / 1024 << " Kb</td><td>" <<  dl->rows() << "</td><td>" << dl->timer.sloop() << "</td><td>" << dl->timer.sfull() << "</td>";
        } else {
            html << "<td colspan=5></td>";
        }
        html << "</tr>\n";

        dl = &data->trunkPriority;
        html << "<tr><th>trunkPriority</th>";
        if (dl->ready()) {
            html << "<td>" << string_time(dl->time()) << "</td><td>" <<  dl->size() / 1024 << " Kb</td><td>" <<  dl->rows() << "</td><td>" << dl->timer.sloop() << "</td><td>" << dl->timer.sfull() << "</td>";
        } else {
            html << "<td colspan=5></td>";
        }
        html << "</tr>\n";

        dl = &data->trunkRule;
        html << "<tr><th>trunkRule</th>";
        if (dl->ready()) {
            html << "<td>" << string_time(dl->time()) << "</td><td>" <<  dl->size() / 1024 << " Kb</td><td>" <<  dl->rows() << "</td><td>" << dl->timer.sloop() << "</td><td>" << dl->timer.sfull() << "</td>";
        } else {
            html << "<td colspan=5></td>";
        }
        html << "</tr>\n";

        dl = &data->client;
        html << "<tr><th>client</th>";
        if (dl->ready()) {
            html << "<td>" << string_time(dl->time()) << "</td><td>" <<  dl->size() / 1024 << " Kb</td><td>" <<  dl->rows() << "</td><td>" << dl->timer.sloop() << "</td><td>" << dl->timer.sfull() << "</td>";
        } else {
            html << "<td colspan=5></td>";
        }
        html << "</tr>\n";

        dl = &data->voipOperator;
        html << "<tr><th>voipOperator</th>";
        if (dl->ready()) {
            html << "<td>" << string_time(dl->time()) << "</td><td>" <<  dl->size() / 1024 << " Kb</td><td>" <<  dl->rows() << "</td><td>" << dl->timer.sloop() << "</td><td>" << dl->timer.sfull() << "</td>";
        } else {
            html << "<td colspan=5></td>";
        }
        html << "</tr>\n";

        dl = &data->pricelist;
        html << "<tr><th>pricelist</th>";
        if (dl->ready()) {
            html << "<td>" << string_time(dl->time()) << "</td><td>" <<  dl->size() / 1024 << " Kb</td><td>" <<  dl->rows() << "</td><td>" << dl->timer.sloop() << "</td><td>" << dl->timer.sfull() << "</td>";
        } else {
            html << "<td colspan=5></td>";
        }
        html << "</tr>\n";

        dl = &data->geoPrefix;
        html << "<tr><th>geoPrefix</th>";
        if (dl->ready()) {
            html << "<td>" << string_time(dl->time()) << "</td><td>" <<  dl->size() / 1024 << " Kb</td><td>" <<  dl->rows() << "</td><td>" << dl->timer.sloop() << "</td><td>" << dl->timer.sfull() << "</td>";
        } else {
            html << "<td colspan=5></td>";
        }
        html << "</tr>\n";




        ol = loader->server.get();
        html << "<tr><th>Server</th>";
        if (ol != 0) {
            html << "<td>" << string_time(ol->loadtime) << "</td><td>" << ol->loadsize / 1024 << " Kb</td><td>" << ol->count << "</td><td>" << ol->t.sloop() << "</td><td>" << ol->t.sfull() << "</td>";
        } else {
            html << "<td colspan=5></td>";
        }
        html << "</tr>\n";

        ol = loader->client.get();
        html << "<tr><th>Client</th>";
        if (ol != 0) {
            html << "<td>" << string_time(ol->loadtime) << "</td><td>" << ol->loadsize / 1024 << " Kb</td><td>" << ol->count << "</td><td>" << ol->t.sloop() << "</td><td>" << ol->t.sfull() << "</td>";
        } else {
            html << "<td colspan=5></td>";
        }
        html << "</tr>\n";

        ol = loader->dest.get();
        html << "<tr><th>Prefix</th>";
        if (ol != 0) {
            html << "<td>" << string_time(ol->loadtime) << "</td><td>" << ol->loadsize / 1024 << " Kb</td><td>" << ol->count << "</td><td>" << ol->t.sloop() << "</td><td>" << ol->t.sfull() << "</td>";
        } else {
            html << "<td colspan=5></td>";
        }
        html << "</tr>\n";

        ol = loader->oper.get();
        html << "<tr><th>Operator</th>";
        if (ol != 0) {
            html << "<td>" << string_time(ol->loadtime) << "</td><td>" << ol->loadsize / 1024 << " Kb</td><td>" << ol->count << "</td><td>" << ol->t.sloop() << "</td><td>" << ol->t.sfull() << "</td>";
        } else {
            html << "<td colspan=5></td>";
        }
        html << "</tr>\n";

        ol = loader->pricelist.get();
        html << "<tr><th>Pricelist</th>";
        if (ol != 0) {
            html << "<td>" << string_time(ol->loadtime) << "</td><td>" << ol->loadsize / 1024 << " Kb</td><td>" << ol->count << "</td><td>" << ol->t.sloop() << "</td><td>" << ol->t.sfull() << "</td>";
        } else {
            html << "<td colspan=5></td>";
        }
        html << "</tr>\n";

        ol = loader->usage.get(tday).get();
        html << "<tr><th>Usage</th>";
        if (ol != 0) {
            html << "<td>" << string_time(ol->loadtime) << "</td><td>" << ol->loadsize / 1024 << " Kb</td><td>" << ol->count << "</td><td>" << ol->t.sloop() << "</td><td>" << ol->t.sfull() << "</td>";
        } else {
            html << "<td colspan=5></td>";
        }
        html << "</tr>\n";

        ol = loader->price.get(tday).get();
        html << "<tr><th>Price</th>";
        if (ol != 0) {
            html << "<td>" << string_time(ol->loadtime) << "</td><td>" << ol->loadsize / 1024 << " Kb</td><td>" << ol->count << "</td><td>" << ol->t.sloop() << "</td><td>" << ol->t.sfull() << "</td>";
        } else {
            html << "<td colspan=5></td>";
        }
        html << "</tr>\n";

        ol = loader->network_prefix.get(tday).get();
        html << "<tr><th>Network prefix</th>";
        if (ol != 0) {
            html << "<td>" << string_time(ol->loadtime) << "</td><td>" << ol->loadsize / 1024 << " Kb</td><td>" << ol->count << "</td><td>" << ol->t.sloop() << "</td><td>" << ol->t.sfull() << "</td>";
        } else {
            html << "<td colspan=5></td>";
        }
        html << "</tr>\n";
    }
    {
        lock_guard<mutex> lock(loader->counter_rwlock);

        ol = loader->counter_client.get();
        html << "<tr><th>Client counter</th>";
        if (ol != 0) {
            html << "<td>" << string_time(ol->loadtime) << "</td><td></td><td>" << ((ClientCounter*) ol)->counter.size() << "</td><td>" << ol->t.sloop() << "</td><td>" << ol->t.sfull() << "</td>";
        } else {
            html << "<td colspan=5></td>";
        }
        html << "</tr>\n";

        ol = loader->counter_fmin.get(tmonth).get();
        html << "<tr><th>Fmin counter</th>";
        if (ol != 0) {
            html << "<td>" << string_time(ol->loadtime) << "</td><td></td><td>" << ((FminCounter*) ol)->counter.size() << "</td><td>" << ol->t.sloop() << "</td><td>" << ol->t.sfull() << "</td>";
        } else {
            html << "<td colspan=5></td>";
        }
        html << "</tr>\n";
    }

    html << "</table>\n";

}

bool ThreadLoader::do_load_data(BDb *db, DataLoader *loader, DataContainer *data) {
    bool success = true;
    time_t tday = get_tday();

    if (loader == 0) loader = this->loader;
    if (data == 0) data = this->data;
    if (db == 0) db = &db_calls;

    ServerList * server = new ServerList();
    ClientList * client = new ClientList();
    DestObjList * dest = new DestObjList();
    OperatorList * oper = new OperatorList();
    PricelistList * pricelist = new PricelistList();
    UsageObjList * usage = new UsageObjList();
    PriceObjList * price = new PriceObjList();
    NetworkPrefixList * network_prefix = new NetworkPrefixList();

    try {
        db_calls.exec("DELETE from event.queue");

        data->loadAll(db);

        server->load(db);

        client->load(db);

        dest->load(db);

        oper->load(db);

        pricelist->load(db);

        usage->load(db, tday);

        price->load(db, tday);

        network_prefix->load(db, tday);

    } catch (Exception &e) {
        e.addTrace("Loader::do_load_data");
        Log::exception(e);
        success = false;
    }

    {
        lock_guard<mutex> lock(loader->rwlock);

        if (server->loaded)
            loader->server = shared_ptr<ServerList>(server);
        else
            delete server;

        if (client->loaded)
            loader->client = shared_ptr<ClientList>(client);
        else
            delete client;

        if (dest->loaded)
            loader->dest = shared_ptr<DestObjList>(dest);
        else
            delete dest;

        if (oper->loaded)
            loader->oper = shared_ptr<OperatorList>(oper);
        else
            delete oper;

        if (pricelist->loaded)
            loader->pricelist = shared_ptr<PricelistList>(pricelist);
        else
            delete pricelist;

        if (usage->loaded)
            loader->usage.addlist(tday, usage);
        else
            delete usage;

        if (price->loaded)
            loader->price.addlist(tday, price);
        else
            delete price;

        if (network_prefix->loaded)
            loader->network_prefix.addlist(tday, network_prefix);
        else
            delete network_prefix;
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
    errors = 0;
}
