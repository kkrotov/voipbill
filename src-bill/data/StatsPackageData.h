#pragma once

#include "BaseData.h"
#include "../lists/StatsPackageCounter.h"

class StatsPackageData : public BaseData<StatsPackageCounter> {
public:
    long long int lastSaveCallTime;

    void init(StatsPackageCounter * item) {
        item->lastSaveCallTime = lastSaveCallTime;
    }

};