#pragma once

#include "BaseData.h"
#include "../lists/ReleaseReasonList.h"

class ReleaseReasonData : public BaseData {

    ObjList * create() {
        return new ReleaseReasonList();
    }

};