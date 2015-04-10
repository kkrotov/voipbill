#pragma once

#include "BaseData.h"
#include "../lists/TrunkPriorityList.h"

class TrunkPriorityData : public BaseData {

    ObjList * create() {
        return new TrunkPriorityList();
    }

};