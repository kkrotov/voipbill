#pragma once

#include "BaseData.h"
#include "../lists/RouteCaseList.h"

class RouteCaseData : public BaseData {

    ObjList * create() {
        return new RouteCaseList();
    }

};