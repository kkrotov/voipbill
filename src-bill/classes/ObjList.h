#pragma once


#include "../common.h"
#include "../classes/BDb.h"
#include "../classes/Timer.h"
#include "../classes/Exception.h"




class BaseObjList {
protected:
    time_t loadtime = 0;
public:
    virtual void load(BDb * db) = 0;
    virtual size_t size() = 0;

    virtual size_t dataSize() {
        return 0;
    };

    time_t loadTime() {
        return loadtime;
    }
};

template <typename T>
class ObjList: public BaseObjList {
protected:
    std::vector<T> data;
    virtual string sql(BDb *db = 0) = 0;
    virtual void parse_item(BDbResult &row, T * item) = 0;
public:

    virtual void load(BDb * db) {
        BDbResult res = db->query(sql(db));

        loadtime = time(NULL);

        this->data.clear();
        this->data.resize(res.size());

        while (res.next()) {
            parse_item(res, &this->data.at(res.position()));
        }
        after_load();
    }

    virtual void after_load() {};

    T * get(size_t i) {
        return &data.at(i);
    }

    size_t dataSize() {
        return sizeof(T) * this->data.size();
    }

    virtual size_t size() {
        return this->data.size();
    }
};
