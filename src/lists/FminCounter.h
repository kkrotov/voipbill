#pragma once

#include "Obj.h"
#include "ObjList.h"
#include <map>
using namespace std;

class FminCounter : public ObjList {
protected:
    virtual size_t item_size();
    virtual string sql(BDb * db);
    virtual void parse_item(BDbResult &row, void * obj);
public:
    map<int, int> counter;
    virtual void load(BDb * db, time_t dt);
    int & get(int usage_id, int group_id);
    int & get(const int key);
    void set(const int usage_id, const int group_id, const int new_value);
    void add(const int usage_id, const int group_id, const int value);
    void clear();
    void append(FminCounter *newfmin);
};
