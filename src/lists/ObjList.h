#pragma once

#include "../common.h"
#include "../classes/BDb.h"
#include "../classes/Timer.h"
#include "../classes/DbException.h"

string string_fmt(const string &fmt, ...);

string string_date(const time_t dt);

string string_time(const time_t dt);

class ObjList {
protected:
	void * data;
	size_t i_size;
	virtual size_t item_size() = 0;
    virtual string sql(BDb *db = 0) = 0;
    virtual void parse_item(BDbResult &row, void * obj) = 0;
public:
    Timer t;
    int count;
    time_t last_use;
    time_t loadtime;
    int loadsize;
    time_t dt;
    bool loaded;

    ObjList();
    ObjList(const ObjList & w);

    virtual ~ObjList();

    virtual void load(BDb * db);
    virtual void load(BDb * db, time_t dt);

    void pre_free();

    void * _get(size_t i);

};

class ObjListByPrefix : public ObjList {
protected:
	virtual char * key(void * obj) = 0;
public:
    virtual void * _find(char * prefix);
};



class ObjListByIntPrefix : public ObjList {
protected:
	virtual int key0(void * obj) = 0;
	virtual char * key(void * obj) = 0;
public:
    virtual void * _find(int value0, char * prefix);
};

class ObjListByInt: public ObjList {
protected:
	virtual int key(void * obj) = 0;
public:
    virtual void * _find(int value);
};


class ObjListBy2Long: public ObjList {
protected:
    virtual long long int key(void * obj) = 0;
public:
    virtual void * _find(long long int value);
};
