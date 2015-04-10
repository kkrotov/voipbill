#pragma once

#include "BaseData.h"
#include "../lists/AirpList.h"

class AirpData : public BaseData {

    ObjList * create() {
        return new AirpList();
    }

};