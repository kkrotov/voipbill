#pragma once

#include "BaseData.h"
#include "../lists/RouteTableList.h"

class RouteTableData : public BaseData {

    ObjList * create() {
        return new RouteTableList();
    }

};