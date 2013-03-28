#pragma once

#include <map>
using namespace std;

#include "../lists/ObjList.h"
#include "../lists/ClientObjList.h"

#include <boost/shared_ptr.hpp>
using boost::shared_ptr;

template <class TypeObjList>
class Loader  {
public:
    bool need_load;
    map<time_t,shared_ptr<TypeObjList> > datamap;

    Loader()
    {
        need_load = true;
    }

    void clear()
    {
        datamap.clear();
    }

    shared_ptr<TypeObjList> get(time_t dt){
        if (datamap.find(dt) != datamap.end())
        {
            shared_ptr<TypeObjList> res = datamap[dt];
            res->last_use = time(NULL);
            return res;
        }
        shared_ptr<TypeObjList> sp_empty;
        return sp_empty;
    }

    virtual shared_ptr<TypeObjList> load(BDb * db, time_t dt, time_t ntime = 0){
        if (ntime == 0) ntime = dt;
        TypeObjList * new_item = new TypeObjList();
        if (need_load)
            try{
                new_item->load(db, ntime);
            }catch( DbException &e ){
                delete new_item;
                if (datamap.find(dt) != datamap.end())
                {
                    return datamap[dt];
                }
                throw e;
            }

        gc();

        shared_ptr<TypeObjList> sp_new_item(new_item);
        datamap[dt] = sp_new_item;
        return sp_new_item;
    }

    virtual void addlist(time_t dt, TypeObjList * new_item){
        shared_ptr<TypeObjList> sp_new_item(new_item);
        datamap[dt] = sp_new_item;
    }

    shared_ptr<TypeObjList> get_or_load(BDb * db, time_t dt, time_t ntime = 0){
        if (datamap.find(dt) != datamap.end())
        {
            shared_ptr<TypeObjList> res = datamap[dt];
            res->last_use = time(NULL);
            return res;
        }
        return load(db, dt, ntime);
    }

    void append(Loader<TypeObjList> & loader2){
        typename map<time_t,shared_ptr<TypeObjList> >::iterator i;
        typename map<time_t,shared_ptr<TypeObjList> >::iterator e;
        for (i = loader2.datamap.begin(), e = loader2.datamap.end() ; i != e; )
        {
            TypeObjList * list2 = i->second.get();
            TypeObjList * list = datamap[i->first].get();
            if (list == 0)
            {
                shared_ptr<TypeObjList> sp_empty(new TypeObjList);
                datamap[i->first] = sp_empty;
                list = sp_empty.get();
            }
            list->append(list2);
            i++;
        }
    }


    static bool timeMoreThen(const time_t &t1, const time_t &t2)
    {
        return t1 > t2;
    }

    void gc()
    {
        list<time_t> keys;
        typename map<time_t,shared_ptr<TypeObjList> >::iterator k;
        for(k = datamap.begin(); k!=datamap.end();k++)
        {
            keys.push_back(k->first);
        }
        keys.sort(Loader::timeMoreThen);
        int timelimit;
        time_t tcurr = time(NULL);
        int i = 0;
        list<time_t>::iterator j = keys.begin();
        while (j != keys.end()){
            if (i == 0)         timelimit = 3600*2;
            else if (i == 1)    timelimit = 30;
            else                timelimit = 5;

            shared_ptr<TypeObjList> item = datamap[*j];
            if ( item->last_use + timelimit < tcurr ){
                datamap.erase(*j);
            }
            i++;
            j++;
        }
    }

};


