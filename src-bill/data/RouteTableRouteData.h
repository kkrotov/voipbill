#pragma once

#include "BaseData.h"
#include "../lists/RouteTableRouteList.h"

class RouteTableRouteData : public BaseData {

    ObjList * create() {
        return new RouteTableRouteList();
    }

};