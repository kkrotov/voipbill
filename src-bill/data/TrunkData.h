#pragma once

#include "BaseData.h"
#include "../lists/TrunkList.h"

class TrunkData : public BaseData {

    ObjList * create() {
        return new TrunkList();
    }

};