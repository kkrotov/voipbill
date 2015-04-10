#include "BaseData.h"

BaseData::BaseData() {
    data = shared_ptr<ObjList>();
}

bool BaseData::ready() {
    lock_guard<mutex> guard(lock);
    return data != 0;
}

shared_ptr<ObjList> BaseData::get() {
    lock_guard<mutex> guard(lock);
    return data;
}

void BaseData::load(BDb * db) {
    TimerScope timerScope(timer);

    shared_ptr<ObjList> tmpData = shared_ptr<ObjList>(create());
    tmpData->load(db);

    lock_guard<mutex> guard(lock);
    data.swap(tmpData);
}

time_t BaseData::time() {
    if (ready()) {
        auto tmpData = get();
        return tmpData->loadtime;
    }
    return 0;
}

int BaseData::rows() {
    if (ready()) {
        auto tmpData = get();
        return tmpData->count;
    }
    return 0;
}

int BaseData::size() {
    if (ready()) {
        auto tmpData = get();
        return tmpData->loadsize;
    }
    return 0;
}
