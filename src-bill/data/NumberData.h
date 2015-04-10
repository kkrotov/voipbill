#pragma once

#include "BaseData.h"
#include "../lists/NumberList.h"

class NumberData : public BaseData {

    ObjList * create() {
        return new NumberList();
    }

};