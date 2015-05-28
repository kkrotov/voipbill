#pragma once

#include "BaseData.h"
#include "../lists/ActiveCounter.h"

class ActiveCounterData : public BaseData<ActiveCounter> {
public:
    void load(shared_ptr<ServiceNumberList> serviceNumberList, shared_ptr<ServiceTrunkList> serviceTrunkList) {
        TimerScope timerScope(timer);

        shared_ptr<ActiveCounter> tmpData = shared_ptr<ActiveCounter>( new ActiveCounter() );
        tmpData.get()->load(serviceNumberList, serviceTrunkList);

        lock_guard<Spinlock> guard(lock);
        data.swap(tmpData);
    }
};