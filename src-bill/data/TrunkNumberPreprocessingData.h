#pragma once

#include "BaseData.h"
#include "../lists/TrunkNumberPreprocessingList.h"

class TrunkNumberPreprocessingData : public BaseData {

    ObjList * create() {
        return new TrunkNumberPreprocessingList();
    }

};