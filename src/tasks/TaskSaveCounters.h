#pragma once

#include "Task.h"
#include "../DataLoader.h"
#include <map>
#include "../KillCalls.h"

class TaskSaveCounters : public Task
{
    Timer t;
    DataLoader *loader;
    BDb db_main;
    BDb db_calls;

    void wait()
    {
        while(app.init_sync_done == false ||
              app.init_load_data_done == false ||
              app.init_load_counters_done == false ||
              app.init_bill_runtime_started == false)
        {
            ssleep(1);
        }
    }

    void prepare()
    {
        while(save_client_counters(true) == false)
        {
            ssleep(10);
        }
    }

    void run()
    {


        while(true){

            t.start();

            save_client_counters();

            save_calls();

            t.stop();

            ssleep(1);

        }
    }

    bool save_client_counters(bool clear = false){

        loader->counter_rwlock.lock();

        shared_ptr<ClientCounter> cl = loader->counter_client;
        if (cl == 0) {
            loader->counter_rwlock.unlock();
            return false;
        }

        string q;
        for (map<int, ClientCounterObj>::iterator i = cl->counter.begin(); i != cl->counter.end(); ++i){
            ClientCounterObj *value = (ClientCounterObj*)&i->second;
            if (value->updated == 0) continue;

            if (q == ""){
                q.append("INSERT INTO billing.clients_counters(client_id,region_id,amount_month,amount_month_sum,amount_day,amount_day_sum,amount_sum,voip_auto_disabled)VALUES");
            }else{
                q.append(",");
            }
            q.append(   string_fmt("(%d,'%d','%s',%d,'%s',%d,%d, %s)",
                                      i->first,
                                      app.conf.geo_region,
                                      string_date(value->amount_month).c_str(),
                                      value->sum_month,
                                      string_date(value->amount_day).c_str(),
                                      value->sum_day,
                                      value->sum,
                                      (value->disabled ? "true" : "false") ) );
            value->updated = 2;

        }
        loader->counter_rwlock.unlock();

        if (q.length() > 0)
        {
            try{
                if (clear)
                {
                    db_main.exec("BEGIN");
                    db_main.exec("DELETE FROM billing.clients_counters WHERE region_id="+lexical_cast<string>(app.conf.geo_region));
                }
                db_main.exec(q);
                if (clear)
                {
                    db_main.exec("COMMIT");
                }
                loader->counter_rwlock.lock(); //.lockForWrite();
                for (map<int, ClientCounterObj>::iterator i = cl->counter.begin(); i != cl->counter.end(); ++i){
                    if (i->second.updated == 2)
                        i->second.updated = 0;
                }
                loader->counter_rwlock.unlock();
            }catch( DbException &e ){
                Log::er(e.what());
                if (clear)
                {
                    try{
                        db_main.exec("ROLLBACK");
                    }catch(...){}
                }
                return false;
            }
        }

        return true;
    }


    bool save_calls(){

        try{
            BDbResult res = db_main.query("select max(id) from billing.calls_"+lexical_cast<string>(app.conf.geo_region));
            res.next();
            long long int max_id = res.get_ll(0);

            BDb::copy("billing.calls_"+lexical_cast<string>(app.conf.geo_region),
                      "",
                      "       id, time, direction_out, usage_num, phone_num, len, usage_id, pricelist_mcn_id, operator_id, free_min_groups_id, dest, mob, redirect, month, day, amount, amount_op, client_id, region, geo_id, pricelist_op_id, price, price_op, srv_region_id",
                      "select id, time, direction_out, usage_num, phone_num, len, usage_id, pricelist_mcn_id, operator_id, free_min_groups_id, dest, mob, redirect, month, day, amount, amount_op, client_id, region, geo_id, pricelist_op_id, price, price_op, "+lexical_cast<string>(app.conf.geo_region)+"::smallint from billing.calls where id>"+lexical_cast<string>(max_id)+" order by id limit 100000",
                      &db_calls, &db_main);

        }catch( DbException &e ){
            Log::er(e.what());
            return false;
        }
        return true;
    }

    void htmlfull(stringstream &html){
        this->html(html);

        html << "Time loop: <b>" << t.sloop() << "</b><br/>\n";
        html << "Time full loop: <b>" << t.sfull() <<  "</b><br/>\n";
        html << "loops: <b>" << t.count << "</b><br/>\n";
        html << "<br/>\n";

    }
public:
    TaskSaveCounters()
    {
        id = "savecounters";
        name = "Save counters";

        loader = DataLoader::instance();

        db_main.setCS(app.conf.db_main);
        db_calls.setCS(app.conf.db_calls);
    }
};
