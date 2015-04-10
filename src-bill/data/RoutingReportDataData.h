#pragma once

#include "BaseData.h"
#include "../lists/RoutingReportDataList.h"

class RoutingReportDataData : public BaseData {

    ObjList * create() {
        return new RoutingReportDataList();
    }

};