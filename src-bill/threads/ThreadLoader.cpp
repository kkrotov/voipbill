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
    ClientObjList * client = 0;
    DestObjList * dest = 0;
    OperatorList * oper = 0;
    PricelistList * pricelist = 0;
    UsageObjList * usage = 0;
    PriceObjList * price = 0;
    NetworkPrefixObjList * network_prefix = 0;

    try {
        BDbResult res = db_calls.query("SELECT event from billing.events");
        while (res.next()) {

            event = res.get_s(0);

            time_t tday = get_tday();

            db_calls.exec("DELETE from billing.events WHERE event='" + event + "'");


            if (event == "clients") {

                client = new ClientObjList();
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

                network_prefix = new NetworkPrefixObjList();
                network_prefix->load(&db_calls, tday);

            }

            boost::this_thread::interruption_point();

        }
    } catch (Exception &e) {

        errors++;
        if (event.length() > 0) {
            try {
                db_calls.exec("INSERT INTO billing.events(event)VALUES('" + event + "')");
            } catch (...) {
            }
            event.clear();
        }

        e.addTrace("Loader::run");
        throw e;
    }

    if (client != 0 || dest != 0 || oper != 0 || pricelist != 0 || usage != 0 || price != 0 || network_prefix != 0) {
        lock_guard<mutex> lock(loader->rwlock);

        if (client != 0) {
            if (client->loaded) loader->client = shared_ptr<ClientObjList>(client);
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
    html << "<br/>\n";


    ObjList * ol;
    time_t tday = get_tday();
    time_t tmonth = get_tmonth();

    {
        lock_guard<mutex> lock(loader->rwlock);

        ol = loader->client.get();
        if (ol != 0) html << "Client: <b>" << string_time(ol->loadtime) << " / " << ol->t.sloop() << " s / " << ol->loadsize / 1024 << " K / " << ol->count << " rows </b><br/>\n";

        ol = loader->dest.get();
        if (ol != 0) html << "Prefix: <b>" << string_time(ol->loadtime) << " / " << ol->t.sloop() << " s / " << ol->loadsize / 1024 << " K / " << ol->count << " rows </b><br/>\n";

        ol = loader->oper.get();
        if (ol != 0) html << "Operator: <b>" << string_time(ol->loadtime) << " / " << ol->t.sloop() << " s / " << ol->loadsize / 1024 << " K / " << ol->count << " rows </b><br/>\n";

        ol = loader->pricelist.get();
        if (ol != 0) html << "Pricelist: <b>" << string_time(ol->loadtime) << " / " << ol->t.sloop() << " s / " << ol->loadsize / 1024 << " K / " << ol->count << " rows </b><br/>\n";

        ol = loader->usage.get(tday).get();
        if (ol != 0) html << "Usage: <b>" << loader->usage.datamap.size() << " / " << string_time(ol->loadtime) << " / " << ol->t.sloop() << " s / " << ol->loadsize / 1024 << " K / " << ol->count << " rows </b><br/>\n";

        ol = loader->price.get(tday).get();
        if (ol != 0) html << "Price: <b>" << loader->price.datamap.size() << " / " << string_time(ol->loadtime) << " / " << ol->t.sloop() << " s / " << ol->loadsize / 1024 << " K / " << ol->count << " rows </b><br/>\n";

        ol = loader->network_prefix.get(tday).get();
        if (ol != 0) html << "Network prefix: <b>" << loader->network_prefix.datamap.size() << " / " << string_time(ol->loadtime) << " / " << ol->t.sloop() << " s / " << ol->loadsize / 1024 << " K / " << ol->count << " rows </b><br/>\n";
    }

    html << "<br/>\n";

    {
        lock_guard<mutex> lock(loader->counter_rwlock);

        ol = loader->counter_client.get();
        if (ol != 0) html << "Client counter: <b>" << string_time(ol->loadtime) << " / " << ol->t.sloop() << " s / " << ((ClientCounter*) ol)->counter.size() << " rows </b><br/>\n";

        ol = loader->counter_fmin.get(tmonth).get();
        if (ol != 0) html << "Fmin counter: <b>" << loader->counter_fmin.datamap.size() << " " << string_time(ol->loadtime) << " / " << ol->t.sloop() << " s / " << ((FminCounter*) ol)->counter.size() << " rows </b><br/>\n";
    }

    html << "<br/>\n";

    html << "Loader errors count: <b>" << errors << "</b><br/>\n";
}

bool ThreadLoader::do_load_data(BDb *db, DataLoader *loader) {
    bool success = true;
    time_t tday = get_tday();

    if (loader == 0) loader = this->loader;
    if (db == 0) db = &db_calls;


    ClientObjList * client = new ClientObjList();
    DestObjList * dest = new DestObjList();
    OperatorList * oper = new OperatorList();
    PricelistList * pricelist = new PricelistList();
    UsageObjList * usage = new UsageObjList();
    PriceObjList * price = new PriceObjList();
    NetworkPrefixObjList * network_prefix = new NetworkPrefixObjList();

    try {
        db_calls.exec("DELETE from billing.events");

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

        if (client->loaded)
            loader->client = shared_ptr<ClientObjList>(client);
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
    db_calls.setCS(app().conf.db_calls);
    errors = 0;
}
