#pragma once

#include "BaseData.h"
#include "../lists/GeoPrefixList.h"

class GeoPrefixData : public BaseData {

    ObjList * create() {
        return new GeoPrefixList();
    }

};