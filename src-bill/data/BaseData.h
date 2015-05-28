#pragma once

#include <mutex>

#include "../../src/common.h"
#include "../../src/classes/Spinlock.h"
#include "../../src/lists/ObjList.h"
#include "../../src/classes/BDb.h"
#include "../../src/classes/Timer.h"

using namespace std;

template <typename T>
class BaseData {
protected:
    shared_ptr<T> data;
    Spinlock lock;
public:
    Timer timer;

    BaseData() {
        data = shared_ptr<T>();
    }

    bool ready() {
        lock_guard<Spinlock> guard(lock);
        return data != 0;
    }

    shared_ptr<T> get() {
        lock_guard<Spinlock> guard(lock);
        return data;
    }

    void load(BDb * db) {
        TimerScope timerScope(timer);

        shared_ptr<T> tmpData = shared_ptr<T>( new T() );
        static_cast<BaseObjList*>(tmpData.get())->load(db);

        lock_guard<Spinlock> guard(lock);
        data.swap(tmpData);
    }

    void swap(shared_ptr<T> tmpData) {
        lock_guard<Spinlock> guard(lock);
        data.swap(tmpData);
    }

    time_t time() {
        if (ready()) {
            auto tmpData = get();
            return static_cast<BaseObjList*>(tmpData.get())->loadTime();
        }
        return 0;
    }

    size_t rows() {
        if (ready()) {
            auto tmpData = get();
            return static_cast<BaseObjList*>(tmpData.get())->size();
        }
        return 0;
    }

    size_t size() {
        if (ready()) {
            auto tmpData = get();
            return static_cast<BaseObjList*>(tmpData.get())->dataSize();
        }
        return 0;
    }
};