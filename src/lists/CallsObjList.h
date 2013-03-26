#pragma once

#include "Obj.h"
#include "ObjList.h"


class CallsObjList : public ObjList {
protected:
	int nrows;
    size_t item_size();
public:
    string ext_filter;
    time_t d_from;
    time_t d_to;
    long long int last_id;
    long long int new_last_id;
    virtual void init(time_t from, time_t to, int rows_count);
    bool loaddata(BDb *db);
    bool next();
    pCallObj get(size_t i);
};
