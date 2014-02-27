#pragma once

#include "ObjList.h"
#include <map>

using namespace std;

class ClientCounter : public ObjList {
protected:
    virtual size_t item_size();
    virtual string sql(BDb * db);
    virtual void parse_item(BDbResult &row, void * obj);
public:
    map<int, ClientCounterObj> counter;

    ClientCounterObj &get(const int client_id);
    void set(int client_id, ClientCounterObj &new_value);
    void clear();
    void append(ClientCounter *newcl);
    void load(BDb * db, time_t dt);
    void reload(BDb * db);
};
