#pragma once

#include <vector>
#include "../classes/BDb.h"
#include "../classes/Timer.h"
#include "../classes/Exception.h"


string string_fmt(const string &fmt, ...);

string string_date(const time_t dt);

string string_time(const time_t dt);

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
class ObjListByPrefix : public ObjList<T> {
protected:
    virtual char * key(T * item) = 0;
public:
    void * _find(const char * prefix) {

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
                char * key_prefix = key(item);

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

template <typename T>
class ObjListByIntPrefix : public ObjList<T> {
protected:
    virtual int key0(T * item) = 0;
    virtual char * key(T * item) = 0;
public:
    void * _find(const int value0, const char * prefix) {
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

template <typename T>
class ObjListByInt : public ObjList<T> {
protected:
    virtual int key(T * item) = 0;
public:
    void * _find(const int value) {
        int low = 0, high = this->data.size() - 1;
        while (low <= high) {
            int mid = (low + high) / 2;

            T * item = &this->data.at(mid);
            int key_value = key(item);

            if (key_value < value)
                low = mid + 1;
            else if (key_value > value)
                high = mid - 1;
            else
                return item;
        }
        return 0;
    }
};

template <typename T>
class ObjListBy2Long : public ObjList<T> {
protected:
    virtual long long int key(T * item) = 0;
public:
    void * _find(long long int value) {
        int low = 0, high = this->data.size() - 1;
        while (low <= high) {
            int mid = (low + high) / 2;

            T * item = &this->data.at(mid);
            long long int key_value = key(item);

            if (key_value < value)
                low = mid + 1;
            else if (key_value > value)
                high = mid - 1;
            else
                return item;
        }
        return 0;
    }
};

template <typename T>
class ObjListByString : public ObjList<T> {
protected:
    virtual char * key(T * item) = 0;
public:
    void * _find(const char *value) {
        int low = 0, high = this->data.size() - 1;
        while (low <= high) {
            int mid = (low + high) / 2;

            T * item = &this->data.at(mid);
            char * key_value = key(item);
            int cmp = strcmp(key_value, value);

            if (cmp < 0)
                low = mid + 1;
            else if (cmp > 0)
                high = mid - 1;
            else
                return item;
        }
        return 0;

    }
};

template <typename T>
class ObjListByIntInt : public ObjList<T> {
protected:
    virtual int key1(T * item) = 0;
    virtual int key2(T * item) = 0;
public:
    void *_find(const int value1, const int value2) {
        int low = 0, high = this->data.size() - 1;
        while (low <= high) {
            int mid = (low + high) / 2;

            T * item = &this->data.at(mid);

            int key_value1 = key1(item);

            if (key_value1 < value1)
                low = mid + 1;
            else if (key_value1 > value1)
                high = mid - 1;
            else {

                int key_value2 = key2(item);

                if (key_value2 < value2)
                    low = mid + 1;
                else if (key_value2 > value2)
                    high = mid - 1;
                else
                    return item;
            }
        }
        return 0;
    }
};

template <typename T>
class ObjListByIntBoolInt : public ObjList<T> {
protected:
    virtual int key1(T * item) = 0;
    virtual bool key2(T * item) = 0;
    virtual int key3(T * item) = 0;
public:
    void *_find(const int value1, const bool value2, const int value3) {
        int low = 0, high = this->data.size() - 1;
        while (low <= high) {
            int mid = (low + high) / 2;

            T * item = &this->data.at(mid);

            int key_value1 = key1(item);

            if (key_value1 < value1)
                low = mid + 1;
            else if (key_value1 > value1)
                high = mid - 1;
            else {

                int key_value2 = key2(item);

                if (key_value2 < value2)
                    low = mid + 1;
                else if (key_value2 > value2)
                    high = mid - 1;
                else {

                    int key_value3 = key3(item);

                    if (key_value3 < value3)
                        low = mid + 1;
                    else if (key_value3 > value3)
                        high = mid - 1;
                    else
                        return item;
                }
            }
        }
        return 0;
    }

};

template <typename T>
class ObjListByIntPeriod : public ObjList<T> {
protected:
    virtual int key1(T * item) = 0;
    virtual time_t key2_from(T * item) = 0;
    virtual time_t key2_to(T * item) = 0;
public:
    void *_find(const int value1, const time_t value2) {
        int low = 0, high = this->data.size() - 1;
        while (low <= high) {
            int mid = (low + high) / 2;

            T * item = &this->data.at(mid);

            int key_value1 = key1(item);

            if (key_value1 < value1)
                low = mid + 1;
            else if (key_value1 > value1)
                high = mid - 1;
            else {

                time_t key_value2_from = key2_from(item);
                time_t key_value2_to = key2_to(item);

                if (key_value2_from > value2)
                    high = mid - 1;
                else if (key_value2_to < value2)
                    low = mid + 1;
                else
                    return item;
            }
        }
        return 0;
    }
};

template <typename T>
class ObjListByLongPeriod : public ObjList<T> {
protected:
    virtual long long int key1(T * item) = 0;
    virtual time_t key2_from(T * item) = 0;
    virtual time_t key2_to(T * item) = 0;
public:

    void *_find(const long long int value1, const time_t value2) {
        int low = 0, high = this->data.size() - 1;
        while (low <= high) {
            int mid = (low + high) / 2;

            T * item = &this->data.at(mid);

            long long int key_value1 = key1(item);

            if (key_value1 < value1)
                low = mid + 1;
            else if (key_value1 > value1)
                high = mid - 1;
            else {

                time_t key_value2_from = key2_from(item);
                time_t key_value2_to = key2_to(item);

                if (key_value2_from > value2)
                    high = mid - 1;
                else if (key_value2_to < value2)
                    low = mid + 1;
                else
                    return item;
            }
        }
        return 0;
    }
};

template <typename T>
class ObjListByIntLongPeriod : public ObjList<T> {
protected:
    virtual int key1(T * item) = 0;
    virtual long long int key2(T * item) = 0;
    virtual time_t key3_from(T * item) = 0;
    virtual time_t key3_to(T * item) = 0;
public:

    void *_find(const int value1, const long long int value2, const time_t value3) {
        int low = 0, high = this->data.size() - 1;
        while (low <= high) {
            int mid = (low + high) / 2;

            T * item = &this->data.at(mid);

            int key_value1 = key1(item);

            if (key_value1 < value1)
                low = mid + 1;
            else if (key_value1 > value1)
                high = mid - 1;
            else {

                long long int key_value2 = key2(item);

                if (key_value2 < value2)
                    low = mid + 1;
                else if (key_value2 > value2)
                    high = mid - 1;
                else {

                    time_t key_value3_from = key3_from(item);
                    time_t key_value3_to = key3_to(item);

                    if (key_value3_from > value3)
                        high = mid - 1;
                    else if (key_value3_to < value3)
                        low = mid + 1;
                    else
                        return item;
                }
            }
        }
        return 0;
    }
};

template <typename T>
class ObjListByStringPeriod : public ObjList<T> {
protected:
    virtual char * key1(T * item) = 0;
    virtual time_t key2_from(T * item) = 0;
    virtual time_t key2_to(T * item) = 0;
public:
    void *_find(const char *value1, const time_t value2) {
        int low = 0, high = this->data.size() - 1;
        while (low <= high) {
            int mid = (low + high) / 2;

            T * item = &this->data.at(mid);

            char * key_value1 = key1(item);
            int cmp = strcmp(key_value1, value1);

            if (cmp < 0)
                low = mid + 1;
            else if (cmp > 0)
                high = mid - 1;
            else {

                time_t key_value2_from = key2_from(item);
                time_t key_value2_to = key2_to(item);

                if (key_value2_from > value2)
                    high = mid - 1;
                else if (key_value2_to < value2)
                    low = mid + 1;
                else
                    return item;
            }
        }
        return 0;
    }
};