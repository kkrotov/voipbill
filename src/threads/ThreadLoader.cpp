#include "ThreadLoader.h"

void ThreadLoader::wait()
{
    while(app.init_sync_done == false)
    {
        ssleep(1);
    }
}

void ThreadLoader::prepare()
{
    while(app.init_load_data_done == false)
    {
        Log::wr("Loading data...");
        if (this->do_load_data() == false)
        {
            ssleep(10);
            continue;
        }
        app.init_load_data_done = true;
    }

    while(app.init_load_counters_done == false)
    {
        Log::wr("Loading counters...");
        if (this->do_load_counters() == false)
        {
            ssleep(10);
            continue;
        }
        app.init_load_counters_done = true;
    }
}

void ThreadLoader::run()
{
    bool counter_locked = false;

    while(true){

        string event;
        ClientObjList * client = 0;
        DestObjList * dest = 0;
        OperatorList * oper = 0;
        UsageObjList * usage = 0;
        PriceObjList * price = 0;

        t.start();

        try{
            BDbResult res = db_calls.query("SELECT event from billing.events");
            while(res.next()){

                event = res.get_s(0);

                time_t tday = get_tday();

                db_calls.exec("DELETE from billing.events WHERE event='"+event+"'");


                if (event == "clients"){

                    client = new ClientObjList();
                    client->load(&db_calls, 0);

                    if (!counter_locked) {
                        counter_locked = true; loader->counter_rwlock.lock(); // .lockForWrite();
                    }

                    shared_ptr<ClientCounter> cc = loader->counter_client;
                    if (cc != 0) cc->reload(&db_calls);

                }else
                if (event == "dest"){

                    dest = new DestObjList();
                    dest->load(&db_calls, 0);

                }else
                if (event == "operator"){

                    oper = new OperatorList();
                    oper->load(&db_calls, 0);

                }else
                if (event == "usage"){

                    usage = new UsageObjList();
                    usage->load(&db_calls, tday);

                }else
                if (event == "price"){

                    price = new PriceObjList();
                    price->load(&db_calls, tday);

                }

                if (counter_locked) { counter_locked = false; loader->counter_rwlock.unlock(); }
            }
        }catch( DbException &e ){
            if (counter_locked) { counter_locked = false; loader->counter_rwlock.unlock(); }
            Log::er(e.what());
            errors++;
            if (event.length() > 0){
                try{
                    db_calls.exec("INSERT INTO billing.events(event)VALUES('"+event+"')");
                }catch(...){}
                event.clear();
            }
        }
        if (client != 0 || dest != 0 || oper != 0 || /*usage_raw != 0 ||*/ usage != 0 || price != 0){
            loader->rwlock.lock(); // .lockForWrite();
            if (client != 0){
                if (client->loaded) loader->client = shared_ptr<ClientObjList>(client); else delete client;
                client = 0;
            }
            if (dest != 0){
                if (dest->loaded) loader->dest = shared_ptr<DestObjList>(dest); else delete dest;
                dest = 0;
            }
            if (oper != 0){
                if (oper->loaded) loader->oper = shared_ptr<OperatorList>(oper); else delete oper;
                oper = 0;
            }

            if (usage != 0){
                if (usage->loaded) loader->usage.addlist(usage->dt, usage); else delete usage;
                usage = 0;
            }
            if (price != 0){
                if (price->loaded) loader->price.addlist(price->dt, price); else delete price;
                price = 0;
            }
            loader->rwlock.unlock();
        }

        t.stop();

        ssleep(1);
    }
}


void ThreadLoader::htmlfull(stringstream &html){
    this->html(html);

    html << "Time loop: <b>" << t.sloop() + "</b><br/>\n";
    html << "Time full loop: <b>" << t.sfull() + "</b><br/>\n";
    html << "loops: <b>" << t.count << "</b><br/>\n";
    html << "<br/>\n";


    ObjList * ol;
    time_t tday = get_tday();
    time_t tmonth = get_tmonth();

    //QReadLocker locker(&loader->rwlock);
    loader->rwlock.lock();



    ol = loader->client.get();
    if (ol != 0) html << "Client: <b>" << string_time(ol->loadtime) << " / " << ol->t.sloop() << " s / " << ol->loadsize/1024 << " K / " << ol->count << " rows </b><br/>\n";

    ol = loader->dest.get();
    if (ol != 0) html << "Prefix: <b>" << string_time(ol->loadtime) << " / " << ol->t.sloop() << " s / " << ol->loadsize/1024 << " K / " << ol->count << " rows </b><br/>\n";

    ol = loader->oper.get();
    if (ol != 0) html << "Operator: <b>" << string_time(ol->loadtime) << " / " << ol->t.sloop() << " s / " << ol->loadsize/1024 << " K / " << ol->count << " rows </b><br/>\n";

    ol = loader->usage.get(tday).get();
    if (ol != 0) html << "Usage: <b>" << loader->usage.datamap.size() << " / " << string_time(ol->loadtime) << " / " << ol->t.sloop() << " s / " << ol->loadsize/1024 << " K / " << ol->count << " rows </b><br/>\n";

    ol = loader->price.get(tday).get();
    if (ol != 0) html << "Price: <b>" << loader->price.datamap.size() << " / " << string_time(ol->loadtime) << " / " << ol->t.sloop() << " s / " << ol->loadsize/1024 << " K / " << ol->count << " rows </b><br/>\n";

    loader->rwlock.unlock();
    //locker.unlock();

    html << "<br/>\n";

    //QReadLocker locker2(&loader->rwlock);
    loader->counter_rwlock.lock();

    ol = loader->counter_client.get();
    if (ol != 0) html << "Client counter: <b>" << string_time(ol->loadtime) << " / " << ol->t.sloop() << " s / " << ((ClientCounter*)ol)->counter.size() << " rows </b><br/>\n";

    ol = loader->counter_fmin.get(tmonth).get();
    if (ol != 0) html << "Fmin counter: <b>" << loader->counter_fmin.datamap.size() << " " << string_time(ol->loadtime) << " / " << ol->t.sloop() << " s / " << ((FminCounter*)ol)->counter.size() << " rows </b><br/>\n";

    loader->counter_rwlock.unlock();
    //locker2.unlock();

    html << "<br/>\n";

    html << "Loader errors count: <b>" << errors << "</b><br/>\n";
}

bool ThreadLoader::do_load_data(){
    bool success = true;
    time_t tday = get_tday();

    ClientObjList * client = new ClientObjList();
    DestObjList * dest = new DestObjList();
    OperatorList * oper = new OperatorList();
    UsageObjList * usage = new UsageObjList();
    PriceObjList * price = new PriceObjList();

    try{
        client->load(&db_calls);

        dest->load(&db_calls);

        oper->load(&db_calls);

        usage->load(&db_calls, tday);

        price->load(&db_calls, tday);

    }catch( DbException &e ){
        Log::er(e.what());
        success = false;
    }

    loader->rwlock.lock();

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

    if (usage->loaded)
        loader->usage.addlist(tday, usage);
    else
        delete usage;

    if (price->loaded)
        loader->price.addlist(tday, price);
    else
        delete price;

    loader->rwlock.unlock();

    return success;
}

bool ThreadLoader::do_load_counters(){
    bool success = true;
    ClientCounter * counter_client = new ClientCounter();
    FminCounter * counter_fmin = new FminCounter();
    time_t t_month = get_tmonth();
    try{

        counter_client->load(&db_calls, 0);

        counter_fmin->load(&db_calls, t_month);

    }catch( DbException &e ){
        Log::er(e.what());
        success = false;
    }

    loader->counter_rwlock.lock();

    if (counter_client->loaded)
        loader->counter_client = shared_ptr<ClientCounter>(counter_client);
    else
        delete counter_client;

    if (counter_fmin->loaded)
        loader->counter_fmin.addlist(t_month, counter_fmin);
    else
        delete counter_fmin;

    loader->counter_rwlock.unlock();

    return success;
}

ThreadLoader::ThreadLoader()
{
    id = "loader";
    name = "Loader";

    loader = DataLoader::instance();
    db_calls.setCS(app.conf.db_calls);
    errors = 0;
}
