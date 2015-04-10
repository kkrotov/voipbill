#pragma once

#include "BaseData.h"
#include "../lists/OperatorList.h"

class OperatorData : public BaseData {

    ObjList * create() {
        return new OperatorList();
    }

};