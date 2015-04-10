#pragma once

#include "BaseData.h"
#include "../lists/OutcomeList.h"

class OutcomeData : public BaseData {

    ObjList * create() {
        return new OutcomeList();
    }

};