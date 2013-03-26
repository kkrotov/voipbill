#pragma once

#include "Obj.h"
#include "ObjList.h"
#include <map>

using namespace std;

class Phone
{
public:
    long long int phone_num;
    int     usage_id;
    int     client_id;
    int     region;
    int     freemin;
    bool    paid_redirect;
    int		pl_local_id;
    int		pl_local_mob_id;
    int		pl_russia_id;
    int		pl_intern_id;
    int		pl_sng_id;
};

class PhoneList : public ObjList {
protected:
    virtual size_t item_size();
    virtual string sql(BDb * db);
    virtual void parse_item(BDbResult &row, void * obj);
public:
    map<int, Phone> list;

    Phone & get(const long long int phone_num);
    void set(long long int phone_num, Phone &new_value);
    void clear();
    void load(BDb * db, time_t dt);
};
