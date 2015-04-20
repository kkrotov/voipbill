#pragma once

#include <vector>
#include <algorithm>
#include "../classes/BDb.h"
#include "../classes/Timer.h"
#include "../classes/Exception.h"


string string_fmt(const string &fmt, ...);

string string_date(time_t dt);

string string_time(time_t dt);

class BaseObjList {
public:
    virtual void load(BDb * db) = 0;
    virtual void load(BDb * db, time_t dt) = 0;
    virtual size_t dataSize() = 0;
    virtual size_t size() = 0;
    virtual time_t loadTime() = 0;
};

template <typename T>
class ObjList: public BaseObjList {
protected:
    time_t loadtime;
    std::vector<T> data;
    virtual string sql(BDb *db = 0) = 0;
    virtual void parse_item(BDbResult &row, T * item) = 0;
public:
    Timer t;
    time_t last_use;
    time_t dt;
    bool loaded;


    ObjList() {
        this->loaded = false;
    }

    virtual ~ObjList() {
    }

    virtual void load(BDb * db) {
        load(db, 0);
    }

    virtual void load(BDb * db, time_t dt) {
        this->dt = dt;

        t.start();

        BDbResult res = db->query(sql(db));

        loadtime = time(NULL);
        last_use = loadtime;

        this->data.clear();
        this->data.resize(res.size());

        while (res.next()) {
            parse_item(res, &this->data.at(res.index));
        }

        this->loaded = true;

        t.stop();
    }

    T * get(size_t i) {
        return &data.at(i);
    }

    size_t dataSize() {
        return sizeof(T) * this->data.size();
    }

    size_t size() {
        return this->data.size();
    }

    time_t loadTime() {
        return loadtime;
    }
};

template <typename T>
class ObjListByIntPrefix : public ObjList<T> {
protected:
    virtual int key0(T * item) = 0;
    virtual char * key(T * item) = 0;
public:
    void * _find(int value0, char * prefix) {
        int lowlow = 0, highhigh = this->data.size() - 1;

        void * res_item = 0;

        size_t lenphone = strlen(prefix);
        for (size_t i = 0; i < lenphone; i++) {
            int low = lowlow, high = highhigh;
            char str[21];
            char *pstr = &str[0];
            char str2[21];
            char *pstr2 = &str2[0];
            memcpy(pstr, prefix, i + 1);
            str[i + 1] = 0;
            while (low <= high) {
                int mid = (low + high) / 2;

                T * item = &this->data.at(mid);
                int key0_value = key0(item);
                char * key_prefix = key(item);


                if (key0_value < value0) {
                    low = mid + 1;
                    lowlow = low;
                    continue;
                } else if (key0_value > value0) {
                    high = mid - 1;
                    highhigh = high;
                    continue;
                }

                int diff = strcmp(key_prefix, pstr);
                if (diff < 0)
                    low = mid + 1;
                else if (diff > 0)
                    high = mid - 1;
                else {
                    res_item = item;
                    break;
                }

                memcpy(pstr2, key_prefix, i + 1);
                str2[i + 1] = 0;
                diff = strcmp(pstr2, pstr);
                if (diff < 0)
                    lowlow = mid + 1;
                else if (diff > 0)
                    highhigh = mid - 1;

            }
        }
        return res_item;

    }

};